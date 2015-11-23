#pragma once

#include <unistd.h>
#include <vector>
#include <string>
#include "utility/maybe.hpp"
#include "pipe.hpp"
#include "io/path.hpp"

namespace gorc {

    class process {
    private:
        maybe<::pid_t> pid;
        maybe<std::unique_ptr<pipe_input_stream>> std_input;
        maybe<std::unique_ptr<pipe_output_stream>> std_output;
        maybe<std::unique_ptr<pipe_output_stream>> std_error;

    public:
        process();
        ~process();

        void redirect_standard_input(std::unique_ptr<pipe_input_stream> &&);
        void redirect_standard_output(std::unique_ptr<pipe_output_stream> &&);
        void redirect_standard_error(std::unique_ptr<pipe_output_stream> &&);

        void execute(path const &executable,
                     std::vector<std::string> const &arguments);

        int join();
    };

}
