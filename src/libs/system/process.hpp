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

namespace gorc {

    class process {
    private:
        maybe<::pid_t> pid;
        maybe<gorc::pipe*> std_input;
        maybe<gorc::pipe*> std_output;
        maybe<gorc::pipe*> std_error;

        void internal_inside_parent(::pid_t child_pid);
        void internal_inside_child(path const &executable,
                                   std::vector<char *> const &arguments);

    public:
        template <typename ArgRangeT>
        process(path const &executable,
                ArgRangeT const &args,
                maybe<gorc::pipe*> std_input,
                maybe<gorc::pipe*> std_output,
                maybe<gorc::pipe*> std_error)
            : std_input(std_input)
            , std_output(std_output)
            , std_error(std_error)
        {
            // LCOV_EXCL_START
            // Not much hope capturing coverage between fork-exec
            pid_t result = ::fork();
            if(result < 0) {
                throw std::system_error(errno, std::generic_category());
            }

            if(result == 0) {
                // Inside child process
                std::string prog_finalname = executable.filename().string();

                // Child address space will be wiped out by exec()
                // Don't need to free arguments.
                std::vector<char *> finalargs;
                finalargs.push_back(&prog_finalname[0]);
                for(auto const &arg : args) {
                    finalargs.push_back(::strdup(arg.c_str()));
                }

                finalargs.push_back(nullptr);
                internal_inside_child(executable, finalargs);
            }
            else {
                // Inside parent process
                internal_inside_parent(result);
            }
            // LCOV_EXCL_STOP
        }

        ~process();
        int join();
    };

}
