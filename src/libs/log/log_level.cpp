#include "log_level.hpp"
#include <stdexcept>

char const* gorc::log_level_to_string(log_level level)
{
    switch(level) {
    case log_level::error:
        return "error";
    case log_level::warning:
        return "warning";
    case log_level::info:
        return "info";
    case log_level::debug:
        return "debug";
    case log_level::trace:
        return "trace";
    }

    throw std::invalid_argument("log_level_to_string unknown log level");
}
