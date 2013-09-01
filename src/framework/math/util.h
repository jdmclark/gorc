#pragma once

#include <cmath>

namespace Gorc {
namespace Math {

template <typename T, typename F> T Lerp(const T& a, const T& b, F alpha) {
	return (1 - alpha) * a + alpha * b;
}

template <typename T> T Clamp(T value, T min, T max) {
	return std::max(min, std::min(value, max));
}

template <typename T> std::tuple<bool, T, T> Quadratic(T a, T b, T c) {
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
