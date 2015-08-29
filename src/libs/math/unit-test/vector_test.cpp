#include "test/test.hpp"
#include "math/vector.hpp"

using namespace gorc;

begin_suite(vector_test);

test_case(vector_default_constructor)
{
    vector<4, int> v;
    for(auto c : v) {
        assert_eq(c, 0);
    }
}

test_case(vector_value_constructor)
{
    vector<4, int> v(1, 2, 3, 4);
    for(int i = 0; i < 4; ++i) {
        assert_eq(v.data[i], (i + 1));
    }
}

test_case(vector_iterators)
{
    vector<4, int> v;
    for(auto &c : v) {
        c = 12;
    }

    vector<4, int> const &w = v;
    for(auto const &c : w) {
        assert_eq(c, 12);
    }
}

test_case(make_vector)
{
    auto v = make_vector(1, 2, 3, 4);
    for(int i = 0; i < 4; ++i) {
        assert_eq(v.data[i], (i + 1));
    }
}

test_case(get)
{
    auto const v = make_vector(85, 48, 12);
    assert_eq(get<0>(v), 85);
    assert_eq(get<1>(v), 48);
    assert_eq(get<2>(v), 12);
}

test_case(get_set)
{
    vector<2, int> v;
    get<0>(v) = 12;
    get<1>(v) = 85;

    assert_eq(v.data[0], 12);
    assert_eq(v.data[1], 85);
}

end_suite(vector_test);
