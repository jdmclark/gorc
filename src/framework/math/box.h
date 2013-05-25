#pragma once

#include "vector.h"
#include <algorithm>

namespace Gorc {
namespace Math {

template <typename F> class Box {
public:
	F Left, Right, Top, Bottom;

public:
	Box() : Left(0), Right(0), Top(0), Bottom(0) {
		return;
	}

	Box(F left, F right, F top, F bottom)
		: Left(left), Right(right), Top(top), Bottom(bottom) {
		return;
	}

	static Box FromSides(F x0, F x1, F y0, F y1) {
		return Box(std::min(x0, x1), std::max(x0, x1), std::max(y0, y1), std::min(y0, y1));
	}

	static Box FromSize(Vector<F> position, F width, F height) {
		return Box(position.X, position.X + width, position.Y + height, position.Y);
	}

	template <typename G> operator Box<G>() const {
		return Box<G>(Left, Right, Top, Bottom);
	}

	Box operator+(Vector<F> offset) {
		return Box(Left + offset.X, Right + offset.X, Top + offset.Y, Bottom + offset.Y);
	}

	Box operator*(F c) {
		return Box(Left * c, Right * c, Top * c, Bottom * c);
	}
};

}
}
