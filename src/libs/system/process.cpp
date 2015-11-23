#include "process.hpp"
#include "log/log.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <system_error>
#include <iostream>

gorc::process::~process()
{
    if(pid.has_value()) {
        LOG_ERROR("zombie process not reaped");
        try {
            join();
// LCOV_EXCL_START
        }
        catch(std::exception const &e) {
            LOG_ERROR(format("error while reaping: %s") % e.what());
        }
// LCOV_EXCL_STOP
    }
}

// LCOV_EXCL_START
// Inside child process between fork and exec
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

void gorc::process::internal_inside_child(path const &executable,
                                          std::vector<char *> const &args)
{
    // Inside child.
    // Close parent ends of pipes.
    // Redirect and close original pipes.

    if(std_input.has_value()) {
        safe_dup2(std_input.get_value()->input->fd, STDIN_FILENO);
        std_input.get_value()->input.reset();
        std_input.get_value()->output.reset();
    }

    if(std_output.has_value()) {
        safe_dup2(std_output.get_value()->output->fd, STDOUT_FILENO);
        std_output.get_value()->input.reset();
        std_output.get_value()->output.reset();
    }

    if(std_error.has_value()) {
        safe_dup2(std_error.get_value()->output->fd, STDERR_FILENO);
        std_error.get_value()->input.reset();
        std_error.get_value()->output.reset();
    }

    std::string prog_name = executable.string();

    ::execvp(prog_name.c_str(), args.data());

    // Still here? Must be an error.
    std::cerr << "[ERROR] Cannot execute: "
              << std::generic_category().message(errno)
              << std::endl;
    exit(126);
}
// LCOV_EXCL_STOP

void gorc::process::internal_inside_parent(::pid_t child_pid)
{
    pid = child_pid;

    // Close child ends of pipe
    if(std_input.has_value()) {
        std_input.get_value()->input.reset();
    }

    if(std_output.has_value()) {
        std_output.get_value()->output.reset();
    }

    if(std_error.has_value()) {
        std_error.get_value()->output.reset();
    }
}

int gorc::process::join()
{
    if(pid.has_value()) {
        int status = 0;
        pid_t result = -1;

        // LCOV_EXCL_START
        do {
            result = ::waitpid(pid.get_value(), &status, 0);
        } while(result < 0 && errno == EINTR);

        if(result < 0) {
            // Essentially uncoverable - system would need to be in a bad state
            throw std::system_error(errno, std::generic_category());
        }
        // LCOV_EXCL_STOP

        pid = nothing;

        if(WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        // LCOV_EXCL_START
        else {
            LOG_ERROR("process terminated abnormally");
            return 1;
        }
        // LCOV_EXCL_STOP
    }
    else {
        throw std::logic_error("no process");
    }
}
