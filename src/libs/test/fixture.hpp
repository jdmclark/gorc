#pragma once

#include "case.hpp"
#include "test_log_backend.hpp"

namespace test {

    class fixture : public case_object {
    public:
        std::queue<std::tuple<gorc::log_level, std::string>> log_message_queue;

        fixture();
        virtual ~fixture();

        virtual void setup();
        virtual void teardown();
    };

}
