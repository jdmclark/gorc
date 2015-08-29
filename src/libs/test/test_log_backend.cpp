#include "test_log_backend.hpp"

test::test_log_backend::test_log_backend(
        std::queue<std::tuple<gorc::log_level, std::string>> &messages)
    : messages(messages)
{
    return;
}

void test::test_log_backend::write_message(std::string const &,
                                           int,
                                           gorc::log_level level,
                                           std::string const &message)
{
    messages.emplace(level, message);
}
