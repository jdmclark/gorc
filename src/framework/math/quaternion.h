#pragma once

#include "vector.h"
#include "util.h"
#include "matrix.h"

namespace gorc {
inline namespace math {

template <typename F> class quaternion {
	template <typename G> friend quaternion<G> make_rotation(const vector<3, G>& axis, G angle);
	template <size_t n, typename G> friend G& get(quaternion<G>& q);
	template <size_t n, typename G> friend G get(const quaternion<G>& q);
	template <typename G> friend quaternion<G> slerp(const quaternion<G>&, const quaternion<G>&, G);
	template <typename G> friend G length(const quaternion<G>&);
	template <typename G> friend G length_squared(const quaternion<G>&);
	template <typename G> friend quaternion<G> invert(const quaternion<G>&);
	template <typename G> friend quaternion<G> normalize(const quaternion<G>&);
private:
	vector<4, F> q;

public:
	quaternion() : q(make_vector(F(1), F(0), F(0), F(0))) {
		return;
	}

	quaternion(const vector<4, F>& q) : q(q) {
		return;
	}

	quaternion<F> operator*(const quaternion<F>& q2) const {
		return quaternion<F>(make_vector(
				get<0>(q) * get<0>(q2.q) - get<1>(q) * get<1>(q2.q) - get<2>(q) * get<2>(q2.q) - get<3>(q) * get<3>(q2.q),
				get<1>(q) * get<0>(q2.q) + get<0>(q) * get<1>(q2.q) + get<2>(q) * get<3>(q2.q) - get<3>(q) * get<2>(q2.q),
				get<0>(q) * get<2>(q2.q) - get<1>(q) * get<3>(q2.q) + get<2>(q) * get<0>(q2.q) + get<3>(q) * get<1>(q2.q),
				get<0>(q) * get<3>(q2.q) + get<1>(q) * get<2>(q2.q) - get<2>(q) * get<1>(q2.q) + get<3>(q) * get<0>(q2.q)));
	}

	const quaternion<F>& operator*=(const quaternion<F>& q2) {
		q = make_vector(
				get<0>(q) * get<0>(q2.q) - get<1>(q) * get<1>(q2.q) - get<2>(q) * get<2>(q2.q) - get<3>(q) * get<3>(q2.q),
				get<1>(q) * get<0>(q2.q) + get<0>(q) * get<1>(q2.q) + get<2>(q) * get<3>(q2.q) - get<3>(q) * get<2>(q2.q),
				get<0>(q) * get<2>(q2.q) - get<1>(q) * get<3>(q2.q) + get<2>(q) * get<0>(q2.q) + get<3>(q) * get<1>(q2.q),
				get<0>(q) * get<3>(q2.q) + get<1>(q) * get<2>(q2.q) - get<2>(q) * get<1>(q2.q) + get<3>(q) * get<0>(q2.q));
		return *this;
	}

	vector<3> transform(const vector<3>& v) const {
		quaternion<F> vq(make_vector(0.0f, get<0>(v), get<1>(v), get<2>(v)));
		auto rq = (*this * vq * conjugate(*this)).q / length_squared(*this);
		return make_vector(get<1>(rq), get<2>(rq), get<3>(rq));
	}
};

template <typename G> quaternion<G> make_rotation(const vector<3, G>& axis, G angle) {
	auto s = sin(to_radians(angle) / G(2));
	return quaternion<G>(make_vector(
			cos(to_radians(angle) / G(2)),
			get<0>(axis) * s,
			get<1>(axis) * s,
			get<2>(axis) * s));
}

template <typename G> quaternion<G> make_euler(const vector<3, G>& v) {
	return make_rotation(make_vector(0.0f, 0.0f, 1.0f), get<1>(v))
			* make_rotation(make_vector(1.0f, 0.0f, 0.0f), get<0>(v))
			* make_rotation(make_vector(0.0f, 1.0f, 0.0f), get<2>(v));
}

template <size_t n, typename G> G& get(quaternion<G>& q) {
	return get<n>(q.q);
}

template <size_t n, typename G> G get(const quaternion<G>& q) {
	return get<n>(q.q);
}

template <typename G> matrix<4, G> convert_to_rotation_matrix(const quaternion<G>& q) {
	auto w = get<0>(q);
	auto x = get<1>(q);
	auto y = get<2>(q);
	auto z = get<3>(q);

	auto sqw = w * w;
	auto sqx = x * x;
	auto sqy = y * y;
	auto sqz = z * z;

	auto invs = G(1) / (sqx + sqy + sqz + sqw);

	auto xy = x * y;
	auto zw = z * w;

	auto m00 = (sqx - sqy - sqz + sqw) * invs;
	auto m11 = (-sqx + sqy - sqz + sqw) * invs;
	auto m22 = (-sqx - sqy + sqz + sqw) * invs;

	auto m10 = G(2) * (xy + zw) * invs;
	auto m01 = G(2) * (xy - zw) * invs;

	auto xz = x * z;
	auto yw = y * w;

	auto m20 = G(2) * (xz - yw) * invs;
	auto m02 = G(2) * (xz + yw) * invs;

	auto yz = y * z;
	auto xw = x * w;

	auto m21 = G(2) * (yz + xw) * invs;
	auto m12 = G(2) * (yz - xw) * invs;

	return make_matrix<4, G>(
			make_vector(m00, m01, m02, G(0)),
			make_vector(m10, m11, m12, G(0)),
			make_vector(m20, m21, m22, G(0)),
			make_vector(G(0), G(0), G(0), G(1))
			);
}

template <typename G> quaternion<G> conjugate(const quaternion<G>& q) {
	return quaternion<G>(make_vector(get<0>(q), -get<1>(q), -get<2>(q), -get<3>(q)));
}

template <typename G> G length_squared(const quaternion<G>& q) {
	return length_squared(q.q);
}

template <typename G> G length(const quaternion<G>& q) {
	return length(q.q);
}

template <typename G> quaternion<G> normalize(const quaternion<G>& q) {
	return quaternion<G>(normalize(q.q));
}

template <typename G> quaternion<G> slerp(const quaternion<G>& a, const quaternion<G>& b, G alpha) {
	double cos_half_theta = dot(a.q, b.q);
	if(abs(cos_half_theta) >= G(1)) {
		return a;
	}

	quaternion<G> mod_a = a;

	// Enforce short rotation
	if(cos_half_theta < G(0)) {
		cos_half_theta = -cos_half_theta;
		mod_a.q = -mod_a.q;
	}

	double half_theta = acos(cos_half_theta);
	double sin_half_theta = sqrt(G(1) - cos_half_theta * cos_half_theta);

	if(fabs(sin_half_theta) < G(0.001)) {
		return quaternion<G>(G(0.5) * (mod_a.q + b.q));
	}

	double ratio_a = sin((G(1) - alpha) * half_theta) / sin_half_theta;
	double ratio_b = sin(alpha * half_theta) / sin_half_theta;

	return quaternion<G>(mod_a.q * ratio_a + b.q * ratio_b);
}

template <typename G> quaternion<G> invert(const quaternion<G>& a) {
	return quaternion<G>(conjugate(a).q / length_squared(a));
}

}
}
