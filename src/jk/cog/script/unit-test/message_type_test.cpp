#include "test/test.hpp"
#include "jk/cog/script/message_type.hpp"

begin_suite(message_type_test);

test_case(invalid_message_type)
{
    assert_throws(gorc::cog::as_string(static_cast<gorc::cog::message_type>(0xFE)),
                  std::range_error,
                  "not a valid message_type");
}

test_case(invalid_message_type_string)
{
    assert_throws_logged(gorc::cog::as_message_type("foobarbaz"));
    assert_log_message(gorc::log_level::error, "unknown message type 'foobarbaz'");
    assert_log_empty();
}

end_suite(message_type_test);
