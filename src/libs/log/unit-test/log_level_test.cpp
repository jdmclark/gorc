#include "test/test.hpp"
#include "log/log_level.hpp"

begin_suite(log_level_test);

test_case(invalid_log_level_to_string)
{
    assert_throws(gorc::log_level_to_string(static_cast<gorc::log_level>(999999)),
                  std::invalid_argument,
                  "log_level_to_string unknown log level");
}

test_case(log_level_to_string_return_values)
{
    assert_eq(gorc::log_level_to_string(gorc::log_level::error),
              std::string("error"));
    assert_eq(gorc::log_level_to_string(gorc::log_level::warning),
              std::string("warning"));
    assert_eq(gorc::log_level_to_string(gorc::log_level::info),
              std::string("info"));
    assert_eq(gorc::log_level_to_string(gorc::log_level::debug),
              std::string("debug"));
    assert_eq(gorc::log_level_to_string(gorc::log_level::trace),
              std::string("trace"));
}

end_suite(log_level_test);
