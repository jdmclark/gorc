#include "test/test.hpp"
#include "utility/string_view.hpp"
#include "utility/string_search.hpp"

using namespace gorc;

begin_suite(string_view_test);

test_case(simple_string_view)
{
    char const *str = "Hello, World!\n";
    std::string substr = "lo, Worl";
    string_view sv(str + 3, 8);

    assert_true(iequal(sv, substr));
}

test_case(user_defined_operator_string_view)
{
    string_view sv = "Hello, World!"_sv;
    std::string str = "Hello, World!";
    assert_true(iequal(sv, str));
}

end_suite(string_view_test);
