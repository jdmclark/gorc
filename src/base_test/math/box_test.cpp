#include "test/test.h"
#include "base/math/box.h"

using namespace gorc;

begin_suite(box_test);

test_case(construct_from_points) {
    auto u = make_vector(10, 10);
    auto v = make_vector(20, 20);
    auto b = make_box(u, v);

    assert_true(b.contains(u));
    assert_true(b.contains(v));
    assert_true(b.contains(make_vector(15, 15)));
    assert_true(!b.contains(make_vector(9, 21)));
    assert_true(!b.contains(make_vector(9, 15)));
    assert_true(!b.contains(make_vector(21, 15)));

    assert_eq(b, make_box(make_vector(10, 10), make_size(10, 10)));
}

test_case(construct_from_size) {
    auto u = make_vector(10, 10);
    auto sz = make_size(10, 10);
    auto b = make_box(u, sz);

    assert_true(b.contains(u));
    assert_true(b.contains(make_vector(20, 20)));
    assert_true(b.contains(make_vector(15, 15)));

    assert_eq(b, make_box(make_vector(10, 10), make_vector(20, 20)));
}

test_case(size) {
    auto u = make_box(make_vector(10, 20), make_vector(20, 30));
    auto v = make_size(get_size<0>(u), get_size<1>(u));

    assert_eq(v, make_size(10, 10));
}

test_case(range) {
    auto u = make_box(make_vector(10, 20), make_vector(20, 30));

    assert_eq(get_range<0>(u), std::make_tuple(10, 20));
    assert_eq(get_range<1>(u), std::make_tuple(20, 30));
}

end_suite(box_test);
