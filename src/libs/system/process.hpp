#pragma once

#include <unistd.h>
#include <vector>
#include <string>
#include <unistd.h>
#include <system_error>
#include <iostream>
#include "utility/maybe.hpp"
#include "pipe.hpp"
#include "io/path.hpp"
#include "argument_list.hpp"

namespace gorc {

    class process {
    private:
        maybe<::pid_t> pid;
        maybe<gorc::pipe*> std_input;
        maybe<gorc::pipe*> std_output;
        maybe<gorc::pipe*> std_error;
        maybe<path> working_directory;

        void internal_inside_parent(::pid_t child_pid);
        void internal_inside_child(path const &executable,
                                   std::vector<char *> const &arguments);

    public:
        process(path const &executable,
                argument_list const &args,
                maybe<gorc::pipe*> std_input,
                maybe<gorc::pipe*> std_output,
                maybe<gorc::pipe*> std_error,
                maybe<path> working_directory = nothing);

        ~process();
        int join();
    };

}
