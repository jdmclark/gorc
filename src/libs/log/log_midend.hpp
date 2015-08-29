#pragma once

#include "log_backend.hpp"
#include "utility/global.hpp"
#include "utility/flag_set.hpp"
#include "log_level.hpp"
#include <mutex>
#include <vector>
#include <tuple>

namespace gorc {

    class log_midend : public global {
        template <typename GlobalT> friend class global_factory;
    private:
        std::vector<std::tuple<flag_set<log_level>, std::unique_ptr<log_backend>>> log_backends;
        std::mutex log_backend_lock;

        log_midend();

    public:
        void insert_log_backend(flag_set<log_level>, std::unique_ptr<log_backend>&&);
        void erase_log_backends();

        void write_log_message(std::string const &filename,
                               int line_number,
                               log_level level,
                               std::string const &message);
    };

}
