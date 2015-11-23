#include "process.hpp"
#include "log/log.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <system_error>

gorc::process::process()
{
    return;
}

gorc::process::~process()
{
    if(pid.has_value()) {
        join();
    }
}

void gorc::process::redirect_standard_input(std::unique_ptr<pipe_input_stream> &&pipe)
{
    std_input = std::forward<std::unique_ptr<pipe_input_stream>>(pipe);
}

void gorc::process::redirect_standard_output(std::unique_ptr<pipe_output_stream> &&pipe)
{
    std_output = std::forward<std::unique_ptr<pipe_output_stream>>(pipe);
}

void gorc::process::redirect_standard_error(std::unique_ptr<pipe_output_stream> &&pipe)
{
    std_error = std::forward<std::unique_ptr<pipe_output_stream>>(pipe);
}

namespace {
    void safe_dup2(int old_fd, int new_fd)
    {
        int result = 0;
        do {
            result = ::dup2(old_fd, new_fd);
        } while(result < 0 && errno == EINTR);

        if(result < 0) {
            std::cerr << "[ERROR] " << std::generic_category().message(errno) << std::endl;
            ::abort();
        }
    }
}

void gorc::process::execute(path const &executable,
                            std::vector<std::string> const &arguments)
{
    if(pid.has_value()) {
        throw std::logic_error("process already exists");
    }

    pid_t result = ::fork();
    if(result < 0) {
        throw std::system_error(errno, std::generic_category());
    }

    if(result == 0) {
        // Inside child
        if(std_input.has_value()) {
            safe_dup2(std_input.get_value()->fd, STDIN_FILENO);
            std_input = nothing;
        }

        if(std_output.has_value()) {
            safe_dup2(std_output.get_value()->fd, STDOUT_FILENO);
            std_output = nothing;
        }

        if(std_error.has_value()) {
            safe_dup2(std_error.get_value()->fd, STDERR_FILENO);
            std_error = nothing;
        }

        std::string prog_name = executable.string();
        std::string prog_finalname = executable.filename().string();

        std::vector<char *> finalargs;
        finalargs.reserve(arguments.size() + 2);

        finalargs.push_back(&prog_finalname[0]);
        for(std::string const &arg : arguments) {
            finalargs.push_back(::strdup(arg.c_str()));
        }

        finalargs.push_back(nullptr);

        ::execvp(prog_name.c_str(), finalargs.data());

        // Still here? Must be an error.
        LOG_ERROR(format("Cannot execute: %s") % std::generic_category().message(errno));

        // Exit 126 - cannot execute
        exit(126);
    }
    else {
        // Inside parent
        pid = result;

        if(std_input.has_value()) {
            std::cerr << "input closed" << std::endl;
            std_input.get_value().reset();
        }

        if(std_output.has_value()) {
            std::cerr << "output closed" << std::endl;
            std_output.get_value().reset();
        }

        if(std_error.has_value()) {
            std::cerr << "error closed" << std::endl;
            std_error.get_value().reset();
        }
    }
}

int gorc::process::join()
{
    if(pid.has_value()) {
        int status = 0;
        pid_t result = -1;

        do {
            result = ::waitpid(pid.get_value(), &status, 0);
        } while(result < 0 && errno == EINTR);

        if(result < 0) {
            throw std::system_error(errno, std::generic_category());
        }

        pid = nothing;

        if(WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        else {
            return 1;
        }
    }
    else {
        throw std::logic_error("no process");
    }
}
