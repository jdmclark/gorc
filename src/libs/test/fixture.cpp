#include "fixture.hpp"
#include "log/log.hpp"

test::fixture::fixture()
{
    gorc::emplace_log_backend<test_log_backend>(
            gorc::flag_set<gorc::log_level> { gorc::log_level::error,
                                              gorc::log_level::warning,
                                              gorc::log_level::info },
            log_message_queue);
    return;
}

test::fixture::~fixture()
{
    gorc::erase_log_backends();
    return;
}

void test::fixture::setup()
{
    return;
}

void test::fixture::teardown()
{
    return;
}
