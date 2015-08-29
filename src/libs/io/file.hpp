#pragma once

#include "output_stream.hpp"

namespace gorc {

    class file : public output_stream {
    public:
        virtual ~file();
    };

}
