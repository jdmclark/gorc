#include "test/test.hpp"
#include "cog/script/string_table.hpp"

using namespace gorc;
using namespace gorc::cog;

begin_suite(string_table_test);

test_case(basic)
{
    string_table st;
    char const *str = st.add_string("Hello, World!");

    assert_eq(std::string(str), std::string("Hello, World!"));
}

end_suite(string_table_test);
