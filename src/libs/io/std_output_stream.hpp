#pragma once

#include "output_stream.hpp"

namespace gorc {

    class std_output_stream : public output_stream {
    public:
        virtual ~std_output_stream();

        virtual size_t write_some(void const *src, size_t size) override;

        void reopen_as_binary();
    };

}
