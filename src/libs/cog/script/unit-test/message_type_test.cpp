#include "test/test.hpp"
#include "cog/script/message_type.hpp"

begin_suite(message_type_test);

test_case(invalid_message_type)
{
    assert_throws(gorc::cog::as_string(static_cast<gorc::cog::message_type>(0xFE)),
                  std::range_error,
                  "not a valid message_type");
}

end_suite(message_type_test);
