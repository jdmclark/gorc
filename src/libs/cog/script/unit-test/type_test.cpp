#include "test/test.hpp"
#include "cog/script/type.hpp"

using namespace gorc::cog;

begin_suite(type_test);

test_case(as_string_out_of_range)
{
    value_type vt = static_cast<value_type>(255);

    assert_throws(as_string(vt),
                  std::range_error,
                  "not a valid value_type");
}

test_case(get_closest_numeric_type)
{
    assert_eq(get_closest_numeric_type(value_type::floating), value_type::floating);
    assert_eq(get_closest_numeric_type(value_type::integer), value_type::integer);
    assert_eq(get_closest_numeric_type(value_type::boolean), value_type::integer);
    assert_eq(get_closest_numeric_type(value_type::sector), value_type::integer);
}

test_case(get_common_numeric_type)
{
    assert_eq(get_common_numeric_type(value_type::floating, value_type::sector),
              value_type::floating);
    assert_eq(get_common_numeric_type(value_type::sector, value_type::floating),
              value_type::floating);
    assert_eq(get_common_numeric_type(value_type::floating, value_type::floating),
              value_type::floating);
    assert_eq(get_common_numeric_type(value_type::integer, value_type::sector),
              value_type::integer);
}

end_suite(type_test);
