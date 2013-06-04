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
};

}
}
