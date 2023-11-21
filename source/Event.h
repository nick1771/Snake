#pragma once

#include <variant>
#include <optional>

enum class Key {
	W,
	A,
	S,
	D,
	Escape,
	Space,
	Unhandled
};

struct KeyPressed {
	Key key{};
};

struct KeyReleased {
	Key key{};
};

struct MouseMove {
	int x{};
	int y{};
};

struct WindowClosed {
};

class Event {
	using EventVariant = std::variant<
		KeyPressed,
		KeyReleased,
		MouseMove,
		WindowClosed
	>;
public:
	template<class T>
	Event(const T& value)
		: _value(value) {
	}

	Event() {
	};

	template<class T>
	std::optional<T> getValue() {
		if (std::holds_alternative<T>(_value)) {
			return std::get<T>(_value);
		}

		return {};
	}
private:
	EventVariant _value{};
};
