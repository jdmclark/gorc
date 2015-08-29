#pragma once

#include "log/log_backend.hpp"
#include <queue>
#include <tuple>

namespace test {
    class test_log_backend : public gorc::log_backend {
    public:
        std::queue<std::tuple<gorc::log_level, std::string>> &messages;

        test_log_backend(std::queue<std::tuple<gorc::log_level, std::string>> &messages);

        virtual void write_message(std::string const &,
                                   int,
                                   gorc::log_level,
                                   std::string const &) override;
    };
}
