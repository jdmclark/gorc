#pragma once

#include "log_backend.hpp"
#include <fstream>

namespace gorc {

    class file_log_backend : public log_backend {
    private:
        std::ofstream file;

    public:
        file_log_backend(std::string const &log_filename);

        virtual void write_message(std::string const &filename,
                                   int line_number,
                                   log_level level,
                                   std::string const &message) override;
    };
}
