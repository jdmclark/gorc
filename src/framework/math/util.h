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

template <typename T> std::tuple<bool, T, T> quadratic(T a, T b, T c) {
	auto D = b * b - 4 * a * c;
	if(D < 0) {
		return std::make_tuple(false, D, D);
	}
	else {
		auto SqrtD = std::sqrt(D);
		auto a2 = 2 * a;

		return std::make_tuple(true, (-b + SqrtD) / a2, (-b - SqrtD) / a2);
	}
}

}
}
