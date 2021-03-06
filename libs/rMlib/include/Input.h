#pragma once

#include "MathUtil.h"

#include <chrono>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

struct libevdev;

namespace rmlib::input {
constexpr static auto max_num_slots = 32;

struct TouchEvent {
  enum { Down, Up, Move } type;
  int id;
  int slot;

  Point location;
  int pressure;
};

// TODO
struct PenEvent {
  enum { TouchDown, TouchUp, ToolClose, ToolLeave, Move } type;
  Point location;
  int distance;
  int pressure;
};

struct KeyEvent {
  enum { Release = 0, Press = 1, Repeat = 2 } type;
  int keyCode;
};

using Event = std::variant<TouchEvent, PenEvent, KeyEvent>;

struct InputManager {
  struct InputDevice {
    int fd;
    libevdev* dev;
    Transform transform;

    int slot = 0;
    std::array<Event, max_num_slots> slots;

    std::unordered_set<int> changedSlots;
    std::vector<Event> events;

    template<typename T>
    T& getSlot() {
      if (!std::holds_alternative<T>(slots[slot])) {
        slots[slot] = T{};
      }
      return std::get<T>(slots[slot]);
    }
  };

  std::optional<int> open(const char* input,
                          Transform inputTransform = Transform::identity());

  /// Opens all devices for the current device type.
  bool openAll();

  void close(int fd);
  void closeAll();

  InputManager() = default;
  ~InputManager() { closeAll(); }

  InputManager(InputManager&& other) : devices(std::move(other.devices)) {
    other.devices.clear();
    other.maxFd = 0;
  }

  InputManager& operator=(InputManager&& other) {
    closeAll();
    std::swap(other, *this);
    return *this;
  }

  InputManager(const InputManager&) = delete;
  InputManager& operator=(const InputManager&) = delete;

  std::optional<std::vector<Event>> waitForInput(
    std::optional<std::chrono::microseconds> timeout = std::nullopt);

  std::optional<std::vector<Event>> readEvents(int);
  static std::optional<std::vector<Event>> readEvent(InputDevice& device);

  void grab();
  void ungrab();
  void flood();

  /// members
  int maxFd = 0;
  std::unordered_map<int, InputDevice> devices;
};

struct SwipeGesture {
  enum Direction { Up, Down, Left, Right };

  Direction direction;
  Point startPosition;
  Point endPosition;
  int fingers;
};

struct PinchGesture {
  enum Direction { In, Out };
  Direction direction;
  Point position;
  int fingers;
};

struct TapGesture {
  int fingers;
  Point position;
};

using Gesture = std::variant<SwipeGesture, PinchGesture, TapGesture>;

struct GestureController {
  // pixels to move before detecting swipe or pinch
  constexpr static int start_threshold = 50;
  constexpr static auto tap_time = std::chrono::milliseconds(150);

  struct SlotState {
    bool active = false;
    Point currentPos;
    Point startPos;
    std::chrono::steady_clock::time_point time;
  };

  Gesture getGesture(Point currentDelta);
  void handleTouchDown(const TouchEvent& event);
  void handleTouchMove(const TouchEvent& event);
  std::optional<Gesture> handleTouchUp(const TouchEvent& event);

  std::vector<Gesture> handleEvents(const std::vector<Event>& events);

  void reset() {
    started = false;
    tapFingers = 0;
  }

  // members
  int currentFinger = 0;
  int tapFingers = 0;

  std::array<SlotState, max_num_slots> slots;

  bool started = false;
  Gesture gesture;
};

} // namespace rmlib::input
