#pragma once

#include <type_traits>
#include <utility>

namespace gorc {
inline namespace utility {

class nothing_type {};
constexpr nothing_type nothing = nothing_type();

class then_do_type {};
constexpr then_do_type then_do = then_do_type();

class else_do_type {};
constexpr else_do_type else_do = else_do_type();

template <typename T, typename Enable = void> class maybe;

template <typename T> class maybe<T, typename std::enable_if<std::is_pointer<T>::value>::type> {
private:
	template <typename U, typename CodeF> friend void if_set(const maybe<U>&, then_do_type, CodeF);
	template <typename U, typename CodeF, typename ElseF> friend void if_set(const maybe<U>&, then_do_type, CodeF, else_do_type, ElseF);

	T value;

	inline auto get_value() const -> decltype(*value) {
		return *value;
	}

public:
	maybe() : value(nullptr) {
		return;
	}

	template <typename U> maybe(maybe<U>&& u) {
		value = u.value;
	}

	maybe(T value) : value(value) {
		return;
	}

	maybe(const nothing_type&) : value(nullptr) {
		return;
	}

	inline operator bool() const {
		return value != nullptr;
	}

	template <typename U> operator maybe<U>() const {
		return maybe<U>(value);
	}

	template <typename U> const maybe<T>& operator=(maybe<U>&& u) {
		value = u.value;
		return *this;
	}

	inline bool operator==(const maybe<T>& u) const {
		return value == u.value;
	}

	inline bool operator!=(const maybe<T>& u) const {
		return value != u.value;
	}
};

template <typename T> class maybe<T, typename std::enable_if<!std::is_pointer<T>::value>::type> {
private:
	template <typename U, typename CodeF> friend void if_set(const maybe<U>&, then_do_type, CodeF);
	template <typename U, typename CodeF, typename ElseF> friend void if_set(const maybe<U>&, then_do_type, CodeF, else_do_type, ElseF);

	T value;
	bool has_value;

	inline T get_value() const {
		return value;
	}

public:
	maybe() : has_value(false) {
		return;
	}

	template <typename U> maybe(maybe<U>&& u) {
		if(u) {
			value = u.value;
			has_value = true;
		}
		else {
			has_value = false;
		}
	}

	maybe(T value) : value(value), has_value(true) {
		return;
	}

	maybe(const nothing_type&) : has_value(false) {
		return;
	}

	inline operator bool() const {
		return has_value;
	}

	template <typename U> const maybe<T>& operator=(maybe<U>&& u) {
		if(u.has_value) {
			value = u.value;
			has_value = true;
		}
		else {
			has_value = false;
		}

		return *this;
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
};

template <typename T> maybe<T> make_maybe(T value) {
	return maybe<T>(value);
}

template <typename U, typename CodeF> void if_set(const maybe<U>& u, then_do_type, CodeF fn) {
	if(u) {
		fn(u.get_value());
	}
}

template <typename U, typename CodeF, typename ElseF> void if_set(const maybe<U>& u, then_do_type, CodeF fn, else_do_type, ElseF gn) {
	if(u) {
		fn(u.get_value());
	}
	else {
		gn();
	}
}

}
}
