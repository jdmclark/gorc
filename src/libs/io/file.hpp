#pragma once

#include "output_stream.hpp"

namespace gorc {

    class file : public output_stream {
    public:
        virtual ~file();

        virtual void set_position(size_t offset) = 0;
        virtual size_t position() = 0;
    };

}
