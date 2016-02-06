#pragma once

#include "log_backend.hpp"

namespace gorc {

    class stdio_log_backend : public log_backend {
    private:
        size_t wrap_width = 80;

    public:
        stdio_log_backend(size_t wrap_width = 80);

        virtual void write_message(std::string const &filename,
                                   int line_number,
                                   log_level level,
                                   std::string const &message) override;
    };
}
