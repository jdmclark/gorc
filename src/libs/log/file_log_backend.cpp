#include "file_log_backend.hpp"

gorc::file_log_backend::file_log_backend(std::string const &filename)
{
    file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    file.open(filename, std::ios_base::out | std::ios_base::app);
}

void gorc::file_log_backend::write_message(std::string const &filename,
                                           int line_number,
                                           log_level level,
                                           std::string const &message)
{
    file << filename << "|"
         << line_number << "|"
         << log_level_to_string(level) << "> "
         << message
         << std::endl;
    file.flush();
}
