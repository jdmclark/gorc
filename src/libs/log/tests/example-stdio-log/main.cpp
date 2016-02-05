#include "log/log.hpp"
#include "log/stdio_log_backend.hpp"

using namespace gorc;

int main(int, char **) noexcept
{
    emplace_log_backend<stdio_log_backend>(
            flag_set<log_level> { log_level::error,
                                  log_level::warning,
                                  log_level::info });

    LOG_ERROR("logged error message");
    LOG_WARNING("logged warning message");
    LOG_INFO("logged info message");
    LOG_INFO(format("logged format message: %s") % "message");

    return EXIT_SUCCESS;
}
