#include "test/test.h"
#include "base/math/vector.h"

using namespace gorc;

begin_suite(vector_test);

test_case(equality_test) {
    assert_true(make_vector(1, 2, 3, 4) == make_vector(1, 2, 3, 4));
}

test_case(inequality_test) {
    assert_true(make_vector(1, 2, 3, 4) != make_vector(1, 1, 3, 4));
}

test_case(iteration_test) {
    auto s = make_vector(0, 1, 2, 3, 4, 5, 6, 7, 8);
    int i = 0;
    for(auto it = s.begin(); it != s.end(); ++it, ++i) {
        assert_eq(*it, i);
    }
}

test_case(const_iteration_test) {
    const auto s = make_vector(0, 1, 2, 3, 4, 5, 6, 7, 8);
    int i = 0;
    for(auto it = s.begin(); it != s.end(); ++it, ++i) {
        assert_eq(*it, i);
    }
}

test_case(range_test) {
    auto s = make_vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5);
    for(auto& c : s) {
        assert_eq(c, 5);
    }
}

test_case(const_range_test) {
    const auto s = make_vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5);
    for(auto& c : s) {
        assert_eq(c, 5);
    }
}

test_case(element_test) {
    auto s = make_vector(1, 2, 3, 4);

    assert_eq(get<0>(s), 1);
    assert_eq(get<1>(s), 2);
    assert_eq(get<2>(s), 3);
    assert_eq(get<3>(s), 4);
}

test_case(const_element_test) {
    const auto s = make_vector(1, 2, 3, 4);

    assert_eq(get<0>(s), 1);
    assert_eq(get<1>(s), 2);
    assert_eq(get<2>(s), 3);
    assert_eq(get<3>(s), 4);
}

test_case(element_set_test) {
    auto s = make_vector(1, 2, 3, 4);

    get<2>(s) = 5;

    assert_eq(get<0>(s), 1);
    assert_eq(get<1>(s), 2);
    assert_eq(get<2>(s), 5);
    assert_eq(get<3>(s), 4);
}

test_case(negation) {
    auto s = make_vector(0, 5, -6);

    assert_eq((-s), make_vector(0, -5, 6));
}

test_case(addition) {
    auto s = make_vector(0, 1, -1);
    auto t = make_vector(1, 1, 1);

    assert_eq((s + t), make_vector(1, 2, 0));
}

test_case(subtraction) {
    auto s = make_vector(0, 1, -1);
    auto t = make_vector(1, 1, 1);

    assert_eq((s - t), make_vector(-1, 0, -2));
}

test_case(scalar_prefix) {
    auto s = make_vector(0, 1, -1);

    assert_eq((3 * s), make_vector(0, 3, -3));
}

test_case(scalar_postfix) {
    auto s = make_vector(0, 1, -1);

    assert_eq((s * 3), make_vector(0, 3, -3));
}

test_case(scalar_division) {
    auto s = make_vector(5, -5, 15);

    assert_eq((s / 5), make_vector(1, -1, 3));
}

test_case(addition_assignment) {
    auto s = make_vector(0, 1, -1);
    auto t = make_vector(1, 1, 1);

    s += t;

    assert_eq(s, make_vector(1, 2, 0));
}

test_case(subtraction_assignment) {
    auto s = make_vector(0, 1, -1);
    auto t = make_vector(1, 1, 1);

    s -= t;

    assert_eq(s, make_vector(-1, 0, -2));
}

test_case(scale_assignment) {
    auto s = make_vector(0, 1, -1);

    s *= 3;

    assert_eq(s, make_vector(0, 3, -3));
}

test_case(scale_division_assignment) {
    auto s = make_vector(5, -5, 15);

    s /= 5;

    assert_eq(s, make_vector(1, -1, 3));
}

test_case(dot_product) {
    auto s = make_vector(0, 1, -1);
    auto t = make_vector(5, 2, 9);

    assert_eq(dot(s, t), -7);
}

test_case(dot_product_orthogonal) {
    auto s = make_vector(10, 10);
    auto t = make_vector(10, -10);

    assert_eq(dot(s, t), 0);
}

test_case(cross_product) {
    auto s = make_vector(0, 1, -1);
    auto t = make_vector(5, 2, 9);

    assert_eq(cross(s, t), make_vector(11, -5, -5));
}

test_case(length_squared) {
    auto s = make_vector(15, 4, 7);
    assert_eq(length_squared(s), 290);
}

test_case(length_squared_dot) {
    auto s = make_vector(15, 4, 7);
    assert_eq(length_squared(s), dot(s, s));
}

test_case(length) {
    auto s = make_vector(4, -4, 2);
    assert_eq(length(s), 6);
}

test_case(normalize) {
    auto s = make_vector(0, 7, 0);
    assert_eq(normalize(s), make_vector(0, 1, 0));

    auto t = make_vector(-16, 0, 0);
    assert_eq(normalize(t), make_vector(-1, 0, 0));
}

test_case(zero_vector) {
    auto s = make_zero_vector<5, float>();

    for(auto c : s) {
        assert_le(c, 0.0f);
        assert_ge(c, 0.0f);
    }
}

test_case(fill_vector) {
    auto s = make_fill_vector<5>(12.0);

    for(auto c : s) {
        assert_le(c, 12.0);
        assert_ge(c, 12.0);
    }
}

test_case(cast_vector) {
    auto s = make_vector(5, 2, 7, 6, -4);
    auto t = static_cast<vector<5, float>>(s);

    assert_true(length(t - make_vector(5.0f, 2.0f, 7.0f, 6.0f, -4.0f)) <= 0.0f);
}

end_suite(vector_test);
