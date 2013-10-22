#pragma once

#include <cmath>

namespace gorc {
inline namespace math {

template <typename T, typename F> T lerp(const T& a, const T& b, F alpha) {
	return (1 - alpha) * a + alpha * b;
}

template <typename T> T clamp(T value, T min, T max) {
	return std::max(min, std::min(value, max));
}

template <typename T> T to_degrees(T value) {
	return value * static_cast<T>(57.2957795);
}

template <typename T> T to_radians(T value) {
	return value * static_cast<T>(0.0174532925);
}

}
}
