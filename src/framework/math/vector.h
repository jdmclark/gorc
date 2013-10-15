#pragma once

#include <cmath>
#include <cstddef>
#include <array>
#include <utility>

#include <iostream>

namespace gorc {
inline namespace math {

template <size_t n, typename F = float> class vector;
template <size_t I, size_t n, typename F> F& get(vector<n, F>& vec);
template <size_t I, size_t n, typename F> F get(const vector<n, F>& vec);
template <size_t n, typename F> F dot(const vector<n, F>& v, const vector<n, F>& w);
template <size_t n, typename F> F length2(const vector<n, F>& v);
template <size_t n, typename F> F length(const vector<n, F>& v);
template <size_t n, typename F> vector<n, F> normalize(const vector<n, F>& v);
template <size_t n, typename F> vector<n, F> operator*(F c, const vector<n, F>& v);
template <typename F> vector<3, F> cross(const vector<3, F>& v, const vector<3, F>& w);

template <size_t n, typename F> class vector {
	template <size_t I, size_t m, typename G> friend G& get(vector<m, G>&);
	template <size_t I, size_t m, typename G> friend G get(const vector<m, G>&);
	template <size_t m, typename G> friend G dot(const vector<m, G>&, const vector<m, G>&);
	template <size_t m, typename G> friend G length2(const vector<m, G>&);
	template <size_t m, typename G> friend G length(const vector<m, G>&);
	template <size_t m, typename G> friend vector<m, G> normalize(const vector<m, G>&);
	template <size_t m, typename G> friend vector<m, G> operator*(G c, const vector<m, G>&);
	template <typename G> friend vector<3, G> cross(const vector<3, G>&, const vector<3, G>&);

private:
	std::array<F, n> data;

public:
	using iterator = decltype(data.begin());
	using const_iterator = decltype(data.cbegin());

	iterator begin() {
		return data.begin();
	}

	const_iterator begin() const {
		return data.begin();
	}

	const_iterator cbegin() const {
		return data.begin();
	}

	iterator end() {
		return data.end();
	}

	const_iterator end() const {
		return data.end();
	}

	const_iterator cend() const {
		return data.end();
	}

	static vector Makevalue(F value = 0) {
		vector rv;
		std::fill(rv.data.begin(), rv.data.end(), value);
		return rv;
	}

	template <typename... V> static vector MakeVector(V... args) {
		static_assert(sizeof...(args) == n, "invalid number of arguments");
		vector rv;
		rv.data = {args...};
		return rv;
	}

	vector operator+(const vector& w) const {
		vector rv;
		for(size_t i = 0; i < n; ++i) {
			rv.data[i] = data[i] + w.data[i];
		}
		return rv;
	}

	const vector& operator+=(const vector& w) {
		for(size_t i = 0; i < n; ++i) {
			data[i] += w.data[i];
		}

		return *this;
	}

	vector operator-(const vector& w) const {
		vector rv;
		for(size_t i = 0; i < n; ++i) {
			rv.data[i] = data[i] - w.data[i];
		}
		return rv;
	}

	const vector& operator-=(const vector& w) {
		for(size_t i = 0; i < n; ++i) {
			data[i] -= w.data[i];
		}

		return *this;
	}

	vector operator-() const {
		vector rv;
		for(size_t i = 0; i < n; ++i) {
			rv.data[i] = -data[i];
		}

		return rv;
	}

	vector operator*(F c) const {
		vector rv;
		for(size_t i = 0; i < n; ++i) {
			rv.data[i] = data[i] * c;
		}
		return rv;
	}

	const vector& operator*=(F c) {
		for(size_t i = 0; i < n; ++i) {
			data[i] *= c;
		}

		return *this;
	}

	vector operator/(F c) const {
		vector rv;
		for(size_t i = 0; i < n; ++i) {
			rv.data[i] = data[i] / c;
		}
		return rv;
	}

	const vector& operator/=(F c) {
		for(size_t i = 0; i < n; ++i) {
			data[i] /= c;
		}

		return *this;
	}

	bool operator==(const vector& v) {
		return data == v.data;
	}

	bool operator!=(const vector& v) {
		return data != v.data;
	}

	template <typename G> operator vector<n, G>() const {
		vector<n, G> rv;
		for(size_t i = 0; i < n; ++i) {
			rv.data[i] = static_cast<G>(data[i]);
		}

		return rv;
	}
};

template <size_t n, typename F> vector<n, F> operator*(F c, const vector<n, F>& v) {
	vector<n, F> rv;
	for(size_t i = 0; i < n; ++i) {
		rv.data[i] = c * v.data[i];
	}

	return rv;
}

const size_t X = 0;
const size_t Y = 1;
const size_t Z = 2;
const size_t W = 3;

template <size_t n, typename F = float> vector<n, F> zero(F value = 0) {
	return vector<n, F>::Makevalue(value);
}

template <typename T, typename... F> vector<sizeof...(F) + 1, T> make_vector(T v1, F... vN) {
	return vector<sizeof...(F) + 1, T>::MakeVector(v1, vN...);
}

template <size_t I, size_t n, typename F> F& get(vector<n, F>& vec) {
	static_assert(I < n, "vector index out of bounds");
	return vec.data[I];
}

template <size_t I, size_t n, typename F> F get(const vector<n, F>& vec) {
	static_assert(I < n, "vector index out of bounds");
	return vec.data[I];
}

template <size_t n, typename F> F dot(const vector<n, F>& v, const vector<n, F>& w) {
	F rv = 0;
	for(size_t i = 0; i < n; ++i) {
		rv += v.data[i] * w.data[i];
	}

	return rv;
}

template <size_t n, typename F> F length2(const vector<n, F>& v) {
	return dot(v, v);
}

template <size_t n, typename F> F length(const vector<n, F>& v) {
	return std::sqrt(length2(v));
}

template <size_t n, typename F> vector<n, F> normalize(const vector<n, F>& v) {
	return v / length(v);
}

template <typename F> vector<3, F> cross(const vector<3, F>& v, const vector<3, F>& w) {
	return make_vector(get<Y>(v) * get<Z>(w) - get<Z>(v) * get<Y>(w),
			get<Z>(v) * get<X>(w) - get<X>(v) * get<Z>(w),
			get<X>(v) * get<Y>(w) - get<Y>(v) * get<X>(w));
}

template <size_t n, typename F> std::ostream& operator<<(std::ostream& os, const vector<n, F>& vec) {
	os << "(";
	auto it = vec.begin();
	if(it != vec.end()) {
		os << *(it++);
		while(it != vec.end()) {
			os << ", " << *(it++);
		}
	}
	os << ")";
	return os;
}

}
}
