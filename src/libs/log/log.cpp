#include "log.hpp"

void gorc::erase_log_backends()
{
    get_global<log_midend>()->erase_log_backends();
}

void gorc::write_log_message(char const *file,
                             int line,
                             log_level level,
                             std::string const &msg)
{
    get_local<log_frontend>()->write_log_message(file, line, level, msg);
}

void gorc::write_log_message(char const *file,
                             int line,
                             log_level level,
                             boost::format const &fmt)
{
    write_log_message(file, line, level, boost::str(fmt));
}
