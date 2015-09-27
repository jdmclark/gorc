#pragma once

#include "log_frontend.hpp"
#include "log_midend.hpp"
#include "log_backend.hpp"
#include "utility/make_unique.hpp"
#include "utility/flag_set.hpp"
#include "logged_runtime_error.hpp"
#include <memory>
#include <string>
#include <cstdlib>
#include <boost/format.hpp>

namespace gorc {

    using boost::format;

    template <typename T, typename... ArgT>
    void emplace_log_backend(flag_set<log_level> filter, ArgT&&... arg)
    {
        get_global<log_midend>()->insert_log_backend(filter,
                                                     make_unique<T>(std::forward<ArgT>(arg)...));
    }

    void erase_log_backends();

    void write_log_message(char const *file,
                           int line,
                           log_level level,
                           std::string const &message);

    void write_log_message(char const *file,
                           int line,
                           log_level level,
                           boost::format const &fmt);

}

#define LOG_WITH_LEVEL(x, y) \
    ::gorc::write_log_message(__FILE__, __LINE__, (x), (y))

#define LOG_ERROR(x) \
    LOG_WITH_LEVEL(::gorc::log_level::error, (x))

#define LOG_WARNING(x) \
    LOG_WITH_LEVEL(::gorc::log_level::warning, (x))

#define LOG_INFO(x) \
    LOG_WITH_LEVEL(::gorc::log_level::info, (x))

#define LOG_DEBUG(x) \
    LOG_WITH_LEVEL(::gorc::log_level::debug, (x))

#define LOG_TRACE(x) \
    LOG_WITH_LEVEL(::gorc::log_level::trace, (x))

#define LOG_FATAL(x) \
    LOG_ERROR(x); throw ::gorc::logged_runtime_error()

#define LOG_FATAL_ASSERT(x, y) \
    do { if(!(x)) { LOG_FATAL(y); } } while(false)
