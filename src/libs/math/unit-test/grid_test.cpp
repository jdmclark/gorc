#include "test/test.hpp"
#include "math/grid.hpp"

using namespace gorc;

begin_suite(grid_test);

test_case(default_fill)
{
    grid<int> g(make_size(2, 2));
    for(size_t y = 0; y < 2; ++y) {
        for(size_t x = 0; x < 2; ++x) {
            assert_eq(g.get(x, y), 0);
        }
    }
}

test_case(simple_fill)
{
    grid<int> g(make_size(2, 2), 12);
    for(size_t y = 0; y < 2; ++y) {
        for(size_t x = 0; x < 2; ++x) {
            assert_eq(g.get(x, y), 12);
        }
    }
}

test_case(simple_get_set)
{
    grid<int> g(make_size(3, 3));

    assert_eq(g.get(2, 2), 0);

    g.get(2, 2) = 5;

    assert_eq(g.get(2, 2), 5);
}

test_case(const_get)
{
    grid<int> g(make_size(3, 3), 42);

    auto const &h = g;

    assert_eq(h.get(2, 2), 42);
}

test_case(width_out_of_bounds)
{
    grid<int> g(make_size(3, 3));

    assert_throws(g.get(3, 2), std::domain_error, "grid::get_element_index out of bounds");
}

test_case(height_out_of_bounds)
{
    grid<int> g(make_size(3, 3));

    assert_throws(g.get(2, 3), std::domain_error, "grid::get_element_index out of bounds");
}

end_suite(grid_test);
