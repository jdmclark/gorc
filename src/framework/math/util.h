#pragma once

#include <cmath>
#include "matrix.h"
#include "vector.h"

namespace gorc {
inline namespace math {

template <typename T, typename F> T lerp(const T& a, const T& b, F alpha) {
	return (1 - alpha) * a + alpha * b;
}

template <typename T> T clamp(T value, T min, T max) {
	return std::max(min, std::min(value, max));
}

template <size_t n, typename F> vector<n, F> clamp_length(const vector<n, F>& value, F min, F max) {
	auto value_len = length(value);
	if(value_len == 0.0f) {
		// Can't do anything. Abort.
		return value;
	}
	else if(value_len < min) {
		return value * (min / value_len);
	}
	else if(value_len > max) {
		return value * (max / value_len);
	}
	else {
		return value;
	}
}

template <typename T> T to_degrees(T value) {
	return value * static_cast<T>(57.2957795);
}

template <typename T> T to_radians(T value) {
	return value * static_cast<T>(0.0174532925);
}

template <typename F> vector<3, F> orient_direction_vector(const vector<3, F>& v, const vector<3, F>& orient) {
	return (make_rotation_matrix(get<1>(orient), make_vector(0.0f, 0.0f, 1.0f))
			* make_rotation_matrix(get<0>(orient), make_vector(1.0f, 0.0f, 0.0f))
			* make_rotation_matrix(get<2>(orient), make_vector(0.0f, 1.0f, 0.0f))).transform_normal(v);
}

}
}