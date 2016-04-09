#include "jk/cog/vm/heap.hpp"
#include "test/test.hpp"

using namespace gorc;
using namespace gorc::cog;

begin_suite(heap_test);

test_case(default_heap)
{
    heap h;
    assert_eq(h.size(), size_t(0));
}

test_case(initialized_heap)
{
    heap h(512);
    assert_eq(h.size(), size_t(512));
}

test_case(access_grows)
{
    heap h;
    h[127];

    assert_eq(h.size(), size_t(128));
}

test_case(const_access_does_not_grow)
{
    heap const h(0);
    h[127];

    assert_eq(h.size(), size_t(0));
}

test_case(round_trip)
{
    heap h;

    for(size_t i = 0; i < 16; ++i) {
        h[i] = value(static_cast<int>(i));
    }

    heap const &g = h;

    for(size_t i = 0; i < 16; ++i) {
        assert_eq(static_cast<int>(g[i]), static_cast<int>(i));
    }

    assert_eq(g[16].get_type(), value_type::nothing);
}

end_suite(heap_test);
