#pragma once

#include <cstdio>
#include <type_traits>

namespace gorc {

    class output_stream;

    class input_stream {
    public:
        virtual ~input_stream();

        void read(void *dest, size_t size);
        virtual size_t read_some(void *dest, size_t size) = 0;
        virtual bool at_end() = 0;

        void copy_to(output_stream &);
    };
}
