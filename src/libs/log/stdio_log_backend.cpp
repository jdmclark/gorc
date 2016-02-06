#include "stdio_log_backend.hpp"
#include "utility/wrapped.hpp"
#include <iostream>

gorc::stdio_log_backend::stdio_log_backend(size_t wrap_width)
    : wrap_width(wrap_width)
{
    return;
}

void gorc::stdio_log_backend::write_message(std::string const & /*filename*/,
                                            int /*line_number*/,
                                            log_level level,
                                            std::string const &message)
{
    char const *tag = "";

    switch(level) {
    case log_level::error:
        tag = "ERROR";
        break;

    case log_level::warning:
        tag = "WARNING";
        break;

    case log_level::debug:
        tag = "DEBUG";
        break;

    default:
        break;
    }

    std::cerr << wrapped(tag, message, wrap_width) << std::endl;
}
