#include "test/test.hpp"
#include "utility/foreach.hpp"
#include <vector>

begin_suite(foreach_test);

test_case(empty_not_called)
{
    int call_ct = 0;

    auto fn = [&](int)
    {
        ++call_ct;
    };

    gorc::foreach(fn);

    assert_eq(call_ct, 0);
}

test_case(call_in_order)
{
    std::vector<int> result;

    auto fn = [&](int v)
    {
        result.push_back(v);
    };

    gorc::foreach(fn, 1, 2, 3, 4, 5, 6);

    std::vector<int> expected = { 1, 2, 3, 4, 5, 6 };

    assert_range_eq(result, expected);
}

end_suite(foreach_test);
