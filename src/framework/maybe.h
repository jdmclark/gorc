#pragma once

#include <type_traits>
#include <utility>

namespace gorc {

template <typename T, typename Enable = void> class maybe;

template <typename T> class maybe<T, typename std::enable_if<std::is_pointer<T>::value>::type> {
private:
	T value;

public:
	maybe() : value(nullptr) {
		return;
	}

	template <typename U> maybe(maybe<U>&& u) {
		U u_value;
		u >> u_value;
		value = u_value;
	}

	explicit maybe(T value) : value(value) {
		return;
	}

	inline operator bool() const {
		return value != nullptr;
	}

	template <typename U> operator maybe<U>() const {
		return maybe<U>(value);
	}

	template <typename U> const maybe<T>& operator=(maybe<U>&& u) {
		U u_value;
		u >> u_value;
		value = u_value;
		return *this;
	}

	inline bool operator>>(T& value_out) const {
		if(value != nullptr) {
			value_out = value;
		}

		return value != nullptr;
	}

	inline bool operator==(const maybe<T>& u) const {
		return value == u.value;
	}

	inline bool operator!=(const maybe<T>& u) const {
		return value != u.value;
	}

	template <typename Fn> void if_set(Fn fn) const {
		if(value != nullptr) {
			fn(*value);
		}
	}
};

template <typename T> class maybe<T, typename std::enable_if<!std::is_pointer<T>::value>::type> {
private:
	T value;
	bool has_value;

public:
	maybe() : has_value(false) {
		return;
	}

	template <typename U> maybe(maybe<U>&& u) {
		if(u) {
			U u_value;
			u >> u_value;
			value = u_value;
			has_value = true;
		}
		else {
			has_value = false;
		}
	}

	explicit maybe(T value) : value(value), has_value(true) {
		return;
	}

	inline operator bool() const {
		return has_value;
	}

	template <typename U> const maybe<T>& operator=(maybe<U>&& u) {
		if(u.has_value) {
			U u_value;
			u >> u_value;
			value = u_value;
			has_value = true;
		}
		else {
			has_value = false;
		}

		return *this;
	}

	inline bool operator>>(T& value_out) const {
		if(has_value) {
			value_out = value;
		}

		return has_value;
	}

	inline bool operator==(const maybe<T>& u) const {
		if(has_value && u.has_value) {
			return value == u.value;
		}

		return has_value == u.has_value;
	}

	inline bool operator!=(const maybe<T>& u) const {
		if(has_value && u.has_value) {
			return value != u.value;
		}

		return has_value != u.has_value;
	}

	template <typename Fn> void if_set(Fn fn) const {
		if(has_value) {
			fn(value);
		}
	}
};

template <typename T> maybe<T> make_maybe(T value) {
	return maybe<T>(value);
}

}
