#pragma once

#include "io/input_stream.hpp"
#include "io/output_stream.hpp"
#include "io/path.hpp"
#include <tuple>
#include <memory>

namespace gorc {

    class pipe_input_stream : public input_stream {
    private:
        bool is_at_end = false;

    public:
        int const fd;

        pipe_input_stream(int fd);
        ~pipe_input_stream();

        virtual size_t read_some(void *dest, size_t size) override;
        virtual bool at_end() override;
    };

    class pipe_output_stream : public output_stream {
    public:
        int const fd;

        pipe_output_stream(int fd);
        ~pipe_output_stream();

        virtual size_t write_some(void const *src, size_t size) override;
    };

    class pipe {
    private:
        std::unique_ptr<pipe_input_stream> input;
        std::unique_ptr<pipe_output_stream> output;

        bool reusable = false;

    public:
        pipe();
        pipe(std::unique_ptr<pipe_input_stream> &&input,
             std::unique_ptr<pipe_output_stream> &&output);

        pipe_input_stream& get_input();
        pipe_output_stream& get_output();

        void close_input();
        void close_output();

        void set_reusable(bool is_reusable);
    };

    pipe make_input_file_pipe(path const &p);
    pipe make_output_file_pipe(path const &p);
    pipe make_output_file_append_pipe(path const &p);

}
