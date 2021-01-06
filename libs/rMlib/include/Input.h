#pragma once

#include "MathUtil.h"

#include <chrono>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace rmlib::input {
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
    Transform transform;

    int slot = 0;
    Event slots[10];

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
  static std::optional<std::vector<Event>> readEvent(InputDevice& device);

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
};

using Gesture = std::variant<SwipeGesture, PinchGesture, TapGesture>;

struct GestureController {
  // pixels to move before detecting swipe or pinch
  constexpr static auto num_slots = 50;
  constexpr static int start_threshold = 50;

  struct SlotState {
    bool active = false;
    Point currentPos;
    Point startPos;
  };

  Gesture getGesture(Point currentDelta);
  void handleTouchDown(const TouchEvent& event);
  void handleTouchMove(const TouchEvent& event);
  std::optional<Gesture> handleTouchUp(const TouchEvent& event);

  std::vector<Gesture> handleEvents(const std::vector<Event>& events);

  void reset() { started = false; }

  int currentFinger = 0;

  std::array<SlotState, num_slots> slots;

  bool started = false;
  Gesture gesture;
};

} // namespace rmlib::input
