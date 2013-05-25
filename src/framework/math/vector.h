#pragma once

#include <cmath>

namespace Gorc {
namespace Math {

template <typename F> class Vector {
public:
	F X, Y, Z;

	Vector() : X(0), Y(0), Z(0) {
		return;
	}

	Vector(F x, F y, F z) : X(x), Y(y), Z(z) {
		return;
	}

	Vector(const Vector& v) {
		*this = v;
	}

	const Vector& operator=(const Vector& v) {
		X = v.X;
		Y = v.Y;
		Z = v.Z;

		return v;
	}

	bool operator==(const Vector& v) const {
		return X == v.X && Y == v.Y && Z == v.Z;
	}

	Vector operator+(const Vector& v) const {
		return Vector(X + v.X, Y + v.Y, Z + v.Z);
	}

	Vector operator-(const Vector& v) const {
		return Vector(X - v.X, Y - v.Y, Z - v.Z);
	}

	Vector operator*(F v) const {
		return Vector(X * v, Y * v, Z * v);
	}

	Vector operator/(F v) const {
		return Vector(X / v, Y / v, Z / v);
	}

	F Length() const {
		return std::sqrt((X * X) + (Y * Y) + (Z * Z));
	}

	static F Dot(const Vector<F>& a, const Vector<F>& b) {
		return (a.X * b.X) + (a.Y * b.Y) + (a.Z * b.Z);
	}

	static Vector Cross(const Vector<F>& a, const Vector<F>& b) {
		return Vector((a.Y * b.Z) - (a.Z * b.Y),
				(a.Z * b.X) - (a.X * b.Z),
				(a.X * b.Y) - (a.Y * b.X));
	}

	template <typename G> operator Vector<G>() const {
		return Vector<G>(X, Y, Z);
	}
};

template <typename F> Vector<F> operator*(F c, const Vector<F>& v) {
	return Vector<F>(c * v.X, c * v.Y, c * v.Z);
}

}
}
