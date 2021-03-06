#include "Input.h"

#include "Device.h"

#include <algorithm>
#include <cassert>

#include <fcntl.h>
#include <unistd.h>

#include <cstring>

#include <libevdev.h>
#include <linux/input.h>

#include <iostream>
#include <memory>

namespace rmlib::input {

std::optional<int>
InputManager::open(const char* input, Transform inputTransform) {
  int fd = ::open(input, O_RDWR | O_NONBLOCK);
  if (fd < 0) {
    return std::nullopt;
  }

  libevdev* dev = nullptr;
  if (libevdev_new_from_fd(fd, &dev) < 0) {
    return std::nullopt;
  }

  devices.emplace(fd, InputDevice{ fd, dev, inputTransform });
  maxFd = std::max(maxFd, fd + 1);
  return fd;
}

bool
InputManager::openAll() {
  auto type = device::getDeviceType();
  if (!type.has_value()) {
    return false;
  }

  auto paths = device::getInputPaths(*type);

  if (!open(paths.touchPath.data(), paths.touchTransform)) {
    return false;
  }

  if (!open(paths.penPath.data(), paths.penTransform)) {
    return false;
  }

  if (!open(paths.buttonPath.data())) {
    return false;
  }

  return true;
}

void
InputManager::close(int fd) {
  auto it = devices.find(fd);
  if (it == devices.end()) {
    return;
  }

  libevdev_free(it->second.dev);
  ::close(fd);

  devices.erase(it);
  maxFd = devices.empty() ? 0
                          : std::max_element(
                              devices.begin(),
                              devices.end(),
                              [](const auto& deva, const auto& devb) {
                                return deva.first < devb.first;
                              })->first +
                              1;
}

void
InputManager::closeAll() {
  for (const auto& [fd, dev] : devices) {
    libevdev_free(dev.dev);
    ::close(fd);
  }
  devices.clear();
  maxFd = 0;
}

std::optional<std::vector<Event>>
InputManager::waitForInput(std::optional<std::chrono::microseconds> timeout) {
  if (devices.empty()) {
    return std::nullopt;
  }

  fd_set fds;
  FD_ZERO(&fds); // NOLINT
  for (auto& [fd, _] : devices) {
    (void)_;
    FD_SET(fd, &fds); // NOLINT
  }

  auto tv = timeval{ 0, 0 };
  if (timeout.has_value()) {
    constexpr auto second_in_usec =
      std::chrono::microseconds(std::chrono::seconds(1)).count();
    tv.tv_sec =
      std::chrono::duration_cast<std::chrono::seconds>(*timeout).count();
    tv.tv_usec = timeout->count() - (tv.tv_sec * second_in_usec);
  }

  auto ret =
    select(maxFd, &fds, nullptr, nullptr, !timeout.has_value() ? nullptr : &tv);
  if (ret < 0) {
    perror("Select on input failed");
    return std::nullopt;
  }

  if (ret == 0) {
    // timeout
    return std::nullopt;
  }

  // Return the first device we see.
  for (auto& [fd, device] : devices) {
    if (!FD_ISSET(fd, &fds)) { // NOLINT
      continue;
    }

    return readEvent(device);
  }

  return std::nullopt;
}

std::optional<std::vector<Event>>
InputManager::readEvents(int fd) {
  auto it = devices.find(fd);
  assert(it != devices.end());
  return readEvent(it->second);
}

namespace {
void
handleEvent(InputManager::InputDevice& device, const input_event& event) {
  if (event.type == EV_SYN && event.code == SYN_REPORT) {
    for (auto slotIdx : device.changedSlots) {
      auto& slot = device.slots.at(slotIdx);
      device.events.push_back(slot);
      std::visit(
        [](auto& r) {
          using Type = std::decay_t<decltype(r)>;
          if constexpr (!std::is_same_v<Type, KeyEvent>) {
            r.type = Type::Move;
          }
        },
        slot);
    }
    device.changedSlots.clear();
    return;
  }

  if (event.type == EV_ABS && event.code == ABS_MT_SLOT) {
    device.slot = event.value;
    device.getSlot<TouchEvent>().slot = event.value;
  }
  device.changedSlots.insert(device.slot);

  if (event.type == EV_ABS) {
    if (event.code == ABS_MT_TRACKING_ID) {
      auto& slot = device.getSlot<TouchEvent>();
      if (event.value == -1) {
        std::cout << "Touch down" << std::endl;
        slot.type = TouchEvent::Up;
        // setType = true;
      } else {
        std::cout << "Touch Up" << std::endl;
        slot.type = TouchEvent::Down;
        slot.id = event.value;
        // setType = true;
      }
    } else if (event.code == ABS_MT_POSITION_X) {
      auto& slot = device.getSlot<TouchEvent>();
      slot.location.x = event.value;
    } else if (event.code == ABS_MT_POSITION_Y) {
      auto& slot = device.getSlot<TouchEvent>();
      slot.location.y = event.value;
    } else if (event.code == ABS_X) {
      device.getSlot<PenEvent>().location.x = event.value;
    } else if (event.code == ABS_Y) {
      device.getSlot<PenEvent>().location.y = event.value;
    } else if (event.code == ABS_DISTANCE) {
      device.getSlot<PenEvent>().distance = event.value;
    } else if (event.code == ABS_PRESSURE) {
      device.getSlot<PenEvent>().pressure = event.value;
    }
  }

  if (event.type == EV_KEY) {
    if (event.code == BTN_TOOL_PEN) {
      if (event.value == KeyEvent::Press) {
        device.getSlot<PenEvent>().type = PenEvent::ToolClose;
        // setType = true;
      } else {
        device.getSlot<PenEvent>().type = PenEvent::ToolLeave;
        // setType = true;
      }
    } else if (event.code == BTN_TOUCH) {
      if (event.value == KeyEvent::Press) {
        device.getSlot<PenEvent>().type = PenEvent::TouchDown;
        // setType = true;
      } else {
        device.getSlot<PenEvent>().type = PenEvent::TouchUp;
        // setType = true;
      }
    } else {
      auto& slot = device.getSlot<KeyEvent>();
      slot.type = static_cast<decltype(slot.type)>(event.value);
      slot.keyCode = event.code;
    }
  }
}
} // namespace

std::optional<std::vector<Event>>
InputManager::readEvent(InputDevice& device) {
  int rc = 0;
  do {
    auto event = input_event{};
    rc = libevdev_next_event(device.dev, LIBEVDEV_READ_FLAG_NORMAL, &event);
    if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
      handleEvent(device, event);
    } else if (rc == LIBEVDEV_READ_STATUS_SYNC) {
      while (rc == LIBEVDEV_READ_STATUS_SYNC) {
        handleEvent(device, event);
        rc = libevdev_next_event(device.dev, LIBEVDEV_READ_FLAG_SYNC, &event);
      }
    }
  } while (rc == LIBEVDEV_READ_STATUS_SYNC ||
           rc == LIBEVDEV_READ_STATUS_SUCCESS);

  auto results = std::move(device.events);
  device.events.clear();

  for (auto& result : results) {
    // Transform result if needed
    std::visit(
      [&device](auto& r) {
        using Type = std::decay_t<decltype(r)>;
        if constexpr (!std::is_same_v<Type, KeyEvent>) {
          r.location = device.transform * r.location;
        }
      },
      result);
  }

  return results;
}

void
InputManager::grab() {
  for (const auto& [_, device] : devices) {
    (void)_;
    libevdev_grab(device.dev, libevdev_grab_mode::LIBEVDEV_GRAB);
  }
}

void
InputManager::ungrab() {
  for (const auto& [_, device] : devices) {
    (void)_;
    libevdev_grab(device.dev, libevdev_grab_mode::LIBEVDEV_UNGRAB);
  }
}

void
InputManager::flood() {
  constexpr auto size = 8 * 512 * 4;
  static const auto* floodBuffer = [] {
    // NOLINTNEXTLINE
    static const auto ret = std::make_unique<input_event[]>(size);

    constexpr auto mk_input_ev = [](int a, int b, int v) {
      input_event r{};
      r.type = a;
      r.code = b;
      r.value = v;
      r.time = { 0, 0 };
      return r;
    };

    for (int i = 0; i < size;) {
      ret[i++] = mk_input_ev(EV_ABS, ABS_DISTANCE, 1);
      ret[i++] = mk_input_ev(EV_SYN, 0, 0);
      ret[i++] = mk_input_ev(EV_ABS, ABS_DISTANCE, 0);
      ret[i++] = mk_input_ev(EV_SYN, 0, 0);
    }

    return ret.get();
  }();

  std::cout << "FLOODING" << std::endl;
  for (const auto& [_, device] : devices) {
    (void)_;
    if (write(device.fd, floodBuffer, size * sizeof(input_event)) == -1) {
      perror("Error writing");
    }
  }
}

namespace {

SwipeGesture::Direction
getSwipeDirection(Point delta) {
  if (std::abs(delta.x) > std::abs(delta.y)) {
    return delta.x > 0 ? SwipeGesture::Right : SwipeGesture::Left;
  }

  return delta.y > 0 ? SwipeGesture::Down : SwipeGesture::Up;
}

PinchGesture::Direction
getPinchDirection() {
  // TODO
  return PinchGesture::In;
}

} // namespace

Gesture
GestureController::getGesture(Point currentDelta) {
  const auto [avgStart, delta] = [this] {
    std::vector<Point> delta;
    Point avgStart;
    int nActive = 0;

#ifndef NDEBUG
    std::cout << "- delta: ";
#endif

    for (const auto& slot : slots) {
      if (slot.active) {
        delta.push_back(slot.currentPos - slot.startPos);

#ifndef NDEBUG
        std::cout << delta.back() << " ";
#endif

        avgStart += slot.startPos;
        nActive++;
      }
    }

#ifndef NDEBUG
    std::cout << std::endl;
#endif

    avgStart /= nActive;
    return std::make_pair(avgStart, std::move(delta));
  }();

  const auto isSwipe =
    std::all_of(
      delta.cbegin(), delta.cend(), [](auto& p) { return p.x >= 0; }) ||
    std::all_of(
      delta.cbegin(), delta.cend(), [](auto& p) { return p.x <= 0; }) ||
    std::all_of(
      delta.cbegin(), delta.cend(), [](auto& p) { return p.y >= 0; }) ||
    std::all_of(delta.cbegin(), delta.cend(), [](auto& p) { return p.y <= 0; });

  if (isSwipe) {
    return SwipeGesture{
      getSwipeDirection(currentDelta), avgStart, /* endPos */ {}, currentFinger
    };
  }

  return PinchGesture{ getPinchDirection(), avgStart, currentFinger };
}

void
GestureController::handleTouchDown(const TouchEvent& event) {
  currentFinger++;

  auto& slot = slots.at(event.slot);
  slot.active = true;
  slot.currentPos = event.location;
  slot.startPos = event.location;
  slot.time = std::chrono::steady_clock::now();

  tapFingers = currentFinger;
}

std::optional<Gesture>
GestureController::handleTouchUp(const TouchEvent& event) {
  std::optional<Gesture> result;

  currentFinger--;

  auto& slot = slots.at(event.slot);
  slot.active = false;

  if (currentFinger == 0) {
    if (!started) {
      // TODO: do we need a time limit?
      // auto delta = std::chrono::steady_clock::now() - slots[event.slot].time;
      // if (delta < tap_time) {
      result = TapGesture{ tapFingers, slot.startPos };
      //}
    } else {
      result = gesture;
    }

    reset();
  }

  return result;
}

void
GestureController::handleTouchMove(const TouchEvent& event) {
  auto& slot = slots.at(event.slot);
  slot.currentPos = event.location;
  auto delta = event.location - slot.startPos;

  if (!started) {
    if (currentFinger >= 2 && (std::abs(delta.x) >= start_threshold ||
                               std::abs(delta.y) >= start_threshold)) {

      started = true;
      gesture = getGesture(delta);
    }

    return;
  }

  // Started, calc percentage (TODO)
}

std::vector<Gesture>
GestureController::handleEvents(const std::vector<Event>& events) {
  std::vector<Gesture> result;

  for (const auto& event : events) {
    if (!std::holds_alternative<TouchEvent>(event)) {
      continue;
    }
    const auto& touchEv = std::get<TouchEvent>(event);
    switch (touchEv.type) {
      case TouchEvent::Down:
        handleTouchDown(touchEv);
        break;
      case TouchEvent::Move:
        handleTouchMove(touchEv);
        break;
      case TouchEvent::Up: {
        auto gesture = handleTouchUp(touchEv);
        if (gesture.has_value()) {
          result.emplace_back(*gesture);
        }
        break;
      }
    }
  }

  return result;
}

} // namespace rmlib::input
