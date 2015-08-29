#include "log/log.hpp"
#include "log/file_log_backend.hpp"
#include <cassert>

using namespace gorc;

int main(int argc, char **argv)
{
    assert(argc == 2);
    emplace_log_backend<file_log_backend>(
            flag_set<log_level> { log_level::error,
                                  log_level::warning,
                                  log_level::info,
                                  log_level::debug,
                                  log_level::trace },
            argv[1]);

    LOG_ERROR("logged error message");
    LOG_WARNING("logged warning message");
    LOG_INFO("logged info message");
    LOG_DEBUG("logged debug message");
    LOG_TRACE("logged trace message");

    return EXIT_SUCCESS;
}
