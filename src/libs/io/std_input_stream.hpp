#pragma once

#include "input_stream.hpp"

namespace gorc {

    class std_input_stream : public input_stream {
    public:
        virtual size_t read_some(void *dest, size_t size) override;
        virtual bool at_end() override;

        void reopen_as_binary();
    };

}
