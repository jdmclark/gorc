#include "log_midend.hpp"

gorc::log_midend::log_midend()
{
    return;
}

void gorc::log_midend::insert_log_backend(flag_set<log_level> filter,
                                          std::unique_ptr<log_backend>&& b)
{
    std::lock_guard<std::mutex> lock(log_backend_lock);
    log_backends.emplace_back(filter, std::move(b));
}

void gorc::log_midend::erase_log_backends()
{
    std::lock_guard<std::mutex> lock(log_backend_lock);
    log_backends.clear();
}

void gorc::log_midend::write_log_message(std::string const &filename,
                                         int line_number,
                                         log_level level,
                                         std::string const &message)
{
    std::lock_guard<std::mutex> lock(log_backend_lock);

    for(auto &b : log_backends) {
        if(std::get<0>(b) & level) {
            std::get<1>(b)->write_message(filename, line_number, level, message);
        }
    }
}
