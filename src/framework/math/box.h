#pragma once

#include <algorithm>
#include "vector.h"

namespace gorc {
inline namespace math {

template <size_t d, typename F = float> class box {
private:
	std::array<std::tuple<F, F>, d> range;

public:
	using iterator = decltype(range.begin());
	using const_iterator = decltype(range.cbegin());

	iterator begin() {
		return range.begin();
	}

	const_iterator begin() const {
		return range.begin();
	}

	const_iterator cbegin() const {
		return range.cbegin();
	}

	iterator end() {
		return range.end();
	}

	const_iterator end() const {
		return range.end();
	}

	const_iterator cend() const {
		return range.cend();
	}

	box() {
		return;
	}

	box(const vector<d, F>& v, const vector<d, F>& w) {
		auto kt = range.begin();
		for(auto it = v.begin(), jt = w.begin(); it != v.end(); ++it, ++jt, ++kt) {
			auto min = std::min(*it, *jt);
			auto max = std::max(*it, *jt);
			*kt = std::make_tuple(min, max);
		}
		return;
	}

	template <unsigned int e> F Size() const {
		const auto& col = std::get<e>(range);
		return std::get<1>(col) - std::get<0>(col);
	}

	template <typename G> operator box<d, G>() const {
		box<d, G> rv;
		auto it = begin();
		auto jt = rv.begin();
		for( ; it != end(); ++it, ++jt) {
			*jt = std::make_tuple(static_cast<G>(std::get<0>(*it)), static_cast<G>(std::get<1>(*it)));
		}

		return rv;
	}

	box<d, F> operator+(const box<d, F>& b) const {
		box<d, F> rv;
		auto rv_it = rv.begin();
		for(auto it = begin(), jt = b.begin(); it != end(); ++rv_it, ++it, ++jt) {
			*rv_it = std::make_tuple(std::min(std::get<0>(*it), std::get<0>(*jt)),
					std::max(std::get<1>(*it), std::get<1>(*jt)));
		}

		return rv;
	}
};

template <size_t d, typename F> bool BoxesOverlap(const box<d, F>& a, const box<d, F>& b) {
	for(auto it = a.begin(), jt = b.begin(); it != a.end(); ++it, ++jt) {
		// Cases: A entirely to left of B, A overlaps B from left,
		// A inside B, A overlaps B from right, A entirely to right of B.
		// Only care about first and last cases.
		if(std::get<1>(*it) < std::get<0>(*jt) || std::get<0>(*it) > std::get<1>(*jt)) {
			return false;
		}
	}

	return true;
}

template <size_t d, typename F> box<d, F> Intersect(const box<d, F>& a, const box<d, F>& b) {
	vector<d, F> min = zero<d>(std::numeric_limits<F>::lowest());
	vector<d, F> max = zero<d>(std::numeric_limits<F>::max());

	auto a_val = a.begin(), b_val = b.begin();
	auto min_val = min.begin(), max_val = max.begin();
	for(; a_val != a.end(); ++a_val, ++b_val, ++min_val, ++max_val) {
		*min_val = std::max(std::get<0>(*a_val), std::get<0>(*b_val));
		*max_val = std::min(std::get<1>(*a_val), std::get<1>(*b_val));
	}

	return box<d, F>(min, max);
}

}
}
