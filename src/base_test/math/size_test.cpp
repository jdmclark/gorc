#include "test/test.h"
#include "base/math/size.h"

using namespace gorc;

begin_suite(size_test);

test_case(equality_test) {
    assert_true(make_size(1, 2, 3, 4) == make_size(1, 2, 3, 4));
}

test_case(inequality_test) {
    assert_true(make_size(1, 2, 3, 4) != make_size(1, 1, 3, 4));
}

test_case(iteration_test) {
    auto s = make_size(0, 1, 2, 3, 4, 5, 6, 7, 8);
    int i = 0;
    for(auto it = s.begin(); it != s.end(); ++it, ++i) {
        assert_eq(*it, i);
    }
}

test_case(const_iteration_test) {
    const auto s = make_size(0, 1, 2, 3, 4, 5, 6, 7, 8);
    int i = 0;
    for(auto it = s.begin(); it != s.end(); ++it, ++i) {
        assert_eq(*it, i);
    }
}

test_case(range_test) {
    auto s = make_size(5, 5, 5, 5, 5, 5, 5, 5, 5, 5);
    for(auto& c : s) {
        assert_eq(c, 5);
    }
}

test_case(const_range_test) {
    const auto s = make_size(5, 5, 5, 5, 5, 5, 5, 5, 5, 5);
    for(auto& c : s) {
        assert_eq(c, 5);
    }
}

test_case(element_test) {
    auto s = make_size(1, 2, 3, 4);

    assert_eq(get<0>(s), 1);
    assert_eq(get<1>(s), 2);
    assert_eq(get<2>(s), 3);
    assert_eq(get<3>(s), 4);
}

test_case(const_element_test) {
    const auto s = make_size(1, 2, 3, 4);

    assert_eq(get<0>(s), 1);
    assert_eq(get<1>(s), 2);
    assert_eq(get<2>(s), 3);
    assert_eq(get<3>(s), 4);
}

test_case(element_set_test) {
    auto s = make_size(1, 2, 3, 4);

    get<2>(s) = 5;

    assert_eq(get<0>(s), 1);
    assert_eq(get<1>(s), 2);
    assert_eq(get<2>(s), 5);
    assert_eq(get<3>(s), 4);
}

test_case(scalar_prefix) {
    auto s = make_size(0, 1, 1);

    assert_eq((3 * s), make_size(0, 3, 3));
}

test_case(scalar_postfix) {
    auto s = make_size(0, 1, 1);

    assert_eq((s * 3), make_size(0, 3, 3));
}

test_case(scalar_division) {
    auto s = make_size(5, 5, 15);

    assert_eq((s / 5), make_size(1, 1, 3));
}

test_case(scale_assignment) {
    auto s = make_size(0, 1, 1);

    s *= 3;

    assert_eq(s, make_size(0, 3, 3));
}

test_case(scale_division_assignment) {
    auto s = make_size(5, 5, 15);

    s /= 5;

    assert_eq(s, make_size(1, 1, 3));
}

test_case(cast_size) {
    auto s = make_size(5, 2, 7, 6, 4);
    auto t = static_cast<size<5, float>>(s);

    assert_eq(t, make_size(5.0f, 2.0f, 7.0f, 6.0f, 4.0f));
}

end_suite(size_test);
