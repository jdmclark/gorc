#include "test/test.hpp"
#include "jk/cog/script/source_type.hpp"

begin_suite(source_type_test);

test_case(invalid_source_type_string)
{
    assert_throws_logged(gorc::cog::as_source_type("foobarbaz"));
    assert_log_message(gorc::log_level::error, "unknown source type 'foobarbaz'");
    assert_log_empty();
}

end_suite(source_type_test);
