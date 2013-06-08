#pragma once

#include <cmath>
#include <cstddef>
#include <array>
#include <utility>

#include <iostream>

namespace Gorc {
namespace Math {

template <size_t n, typename F = float> class Vector;
template <size_t I, size_t n, typename F> F& Get(Vector<n, F>& vec);
template <size_t I, size_t n, typename F> F Get(const Vector<n, F>& vec);
template <size_t n, typename F> F Dot(const Vector<n, F>& v, const Vector<n, F>& w);
template <size_t n, typename F> F Length2(const Vector<n, F>& v);
template <size_t n, typename F> F Length(const Vector<n, F>& v);
template <size_t n, typename F> Vector<n, F> Normalize(const Vector<n, F>& v);
template <size_t n, typename F> Vector<n, F> operator*(F c, const Vector<n, F>& v);
template <typename F> Vector<3, F> Cross(const Vector<3, F>& v, const Vector<3, F>& w);

template <size_t n, typename F> class Vector {
	template <size_t I, size_t m, typename G> friend G& Get(Vector<m, G>&);
	template <size_t I, size_t m, typename G> friend G Get(const Vector<m, G>&);
	template <size_t m, typename G> friend G Dot(const Vector<m, G>&, const Vector<m, G>&);
	template <size_t m, typename G> friend G Length2(const Vector<m, G>&);
	template <size_t m, typename G> friend G Length(const Vector<m, G>&);
	template <size_t m, typename G> friend Vector<m, G> Normalize(const Vector<m, G>&);
	template <size_t m, typename G> friend Vector<m, G> operator*(G c, const Vector<m, G>&);
	template <typename G> friend Vector<3, G> Cross(const Vector<3, G>&, const Vector<3, G>&);

private:
	std::array<F, n> data;

public:
	static Vector MakeZero() {
		Vector rv;
		std::fill(rv.data.begin(), rv.data.end(), 0);
		return rv;
	}

	template <typename... V> static Vector MakeVector(V... args) {
		static_assert(sizeof...(args) == n, "invalid number of arguments");
		Vector rv;
		rv.data = {args...};
		return rv;
	}

	Vector operator+(const Vector& w) const {
		Vector rv;
		for(size_t i = 0; i < n; ++i) {
			rv.data[i] = data[i] + w.data[i];
		}
		return rv;
	}

	const Vector& operator+=(const Vector& w) {
		for(size_t i = 0; i < n; ++i) {
			data[i] += w.data[i];
		}

		return *this;
	}

	Vector operator-(const Vector& w) const {
		Vector rv;
		for(size_t i = 0; i < n; ++i) {
			rv.data[i] = data[i] - w.data[i];
		}
		return rv;
	}

	const Vector& operator-=(const Vector& w) {
		for(size_t i = 0; i < n; ++i) {
			data[i] -= w.data[i];
		}

		return *this;
	}

	Vector operator*(F c) const {
		Vector rv;
		for(size_t i = 0; i < n; ++i) {
			rv.data[i] = data[i] * c;
		}
		return rv;
	}

	const Vector& operator*=(F c) {
		for(size_t i = 0; i < n; ++i) {
			data[i] *= c;
		}

		return *this;
	}

	Vector operator/(F c) const {
		Vector rv;
		for(size_t i = 0; i < n; ++i) {
			rv.data[i] = data[i] / c;
		}
		return rv;
	}

	const Vector& operator/=(F c) {
		for(size_t i = 0; i < n; ++i) {
			data[i] /= c;
		}

		return *this;
	}

	bool operator==(const Vector& v) {
		return data == v.data;
	}

	bool operator!=(const Vector& v) {
		return data != v.data;
	}

	template <typename G> operator Vector<n, G>() const {
		Vector<n, G> rv;
		for(size_t i = 0; i < n; ++i) {
			rv.data[i] = static_cast<G>(data[i]);
		}

		return rv;
	}
};

template <size_t n, typename F> Vector<n, F> operator*(F c, const Vector<n, F>& v) {
	Vector<n, F> rv;
	for(size_t i = 0; i < n; ++i) {
		rv.data[i] = c * v.data[i];
	}

	return rv;
}

const size_t X = 0;
const size_t Y = 1;
const size_t Z = 2;
const size_t W = 3;

template <size_t n, typename F = float> Vector<n, F> Zero() {
	return Vector<n, F>();
}

template <typename T, typename... F> Vector<sizeof...(F) + 1, T> Vec(T v1, F... vN) {
	return Vector<sizeof...(F) + 1, T>::MakeVector(v1, vN...);
}

template <size_t I, size_t n, typename F> F& Get(Vector<n, F>& vec) {
	static_assert(I < n, "vector index out of bounds");
	return vec.data[I];
}

template <size_t I, size_t n, typename F> F Get(const Vector<n, F>& vec) {
	static_assert(I < n, "vector index out of bounds");
	return vec.data[I];
}

template <size_t n, typename F> F Dot(const Vector<n, F>& v, const Vector<n, F>& w) {
	F rv = 0;
	for(size_t i = 0; i < n; ++i) {
		rv += v.data[i] * w.data[i];
	}

	return rv;
}

template <size_t n, typename F> F Length2(const Vector<n, F>& v) {
	return Dot(v, v);
}

template <size_t n, typename F> F Length(const Vector<n, F>& v) {
	return std::sqrt(Length2(v));
}

template <size_t n, typename F> Vector<n, F> Normalize(const Vector<n, F>& v) {
	return v / Length(v);
}

template <typename F> Vector<3, F> Cross(const Vector<3, F>& v, const Vector<3, F>& w) {
	return Vec(Get<Y>(v) * Get<Z>(w) - Get<Z>(v) * Get<Y>(w),
			Get<Z>(v) * Get<X>(w) - Get<X>(v) * Get<Z>(w),
			Get<X>(v) * Get<Y>(w) - Get<Y>(v) * Get<X>(w));
}

}
}
