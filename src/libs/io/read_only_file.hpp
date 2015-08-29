#pragma once

#include "input_stream.hpp"

namespace gorc {

    class read_only_file : public input_stream {
    public:
        virtual ~read_only_file();

        virtual void seek(ssize_t offset) = 0;
        virtual void set_position(size_t offset) = 0;
        virtual size_t position() = 0;

        virtual size_t size() = 0;
    };

}
