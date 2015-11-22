#pragma once

#include "io/input_stream.hpp"
#include "io/output_stream.hpp"
#include <tuple>
#include <memory>

namespace gorc {

    class pipe_input_stream : public input_stream {
    private:
        bool is_at_end = false;

    public:
        const int fd;

        pipe_input_stream(int fd);
        ~pipe_input_stream();

        virtual size_t read_some(void *dest, size_t size) override;
        virtual bool at_end() override;
    };

    class pipe_output_stream : public output_stream {
    private:
        int fd;

    public:
        pipe_output_stream(int fd);
        ~pipe_output_stream();

        virtual size_t write_some(void const *src, size_t size) override;
    };

    class pipe {
    public:
        pipe();

        std::unique_ptr<pipe_input_stream> input;
        std::unique_ptr<pipe_output_stream> output;
    };

}
