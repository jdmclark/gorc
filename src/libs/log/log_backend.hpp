#pragma once

#include "log_level.hpp"
#include <string>

namespace gorc {

    class log_backend {
    public:
        virtual ~log_backend();

        virtual void write_message(std::string const &filename,
                                   int line_number,
                                   log_level level,
                                   std::string const &message) = 0;
    };

}
