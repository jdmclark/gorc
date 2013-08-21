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

}
}
