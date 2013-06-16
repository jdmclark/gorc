#pragma once

#include <algorithm>
#include "vector.h"

namespace Gorc {
namespace Math {

template <size_t d, typename F = float> class Box {
private:
	Vector<d, F> v, w;

public:
	Box() {
		return;
	}

	Box(const Vector<d, F>& v, const Vector<d, F>& w) : v(v), w(w) {
		return;
	}

	template <size_t e> F Size() const {
		auto vx = Get<e>(v);
		auto wx = Get<e>(w);
		return std::max(vx, wx) - std::min(vx, wx);
	}
};

}
}
