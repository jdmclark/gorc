#include "argument_list.hpp"
#include "process.hpp"
#include "log/log.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <system_error>
#include <iostream>
#include <boost/filesystem.hpp>

gorc::process::process(path const &executable,
                       argument_list const &args,
                       maybe<gorc::pipe*> std_input,
                       maybe<gorc::pipe*> std_output,
                       maybe<gorc::pipe*> std_error,
                       maybe<path> working_directory)
    : std_input(std_input)
    , std_output(std_output)
    , std_error(std_error)
    , working_directory(working_directory)
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

// LCOV_EXCL_START
gorc::process::~process()
{
    if(pid.has_value()) {
        LOG_ERROR("zombie process not reaped");
        try {
            join();
        }
        catch(std::exception const &e) {
            LOG_ERROR(format("error while reaping: %s") % e.what());
        }
    }
}
// LCOV_EXCL_STOP

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
        safe_dup2(std_input.get_value()->get_input().fd, STDIN_FILENO);
        std_input.get_value()->close_input();
        std_input.get_value()->close_output();
    }

    if(std_output.has_value()) {
        safe_dup2(std_output.get_value()->get_output().fd, STDOUT_FILENO);
        std_output.get_value()->close_input();
        std_output.get_value()->close_output();
    }

    if(std_error.has_value()) {
        safe_dup2(std_error.get_value()->get_output().fd, STDERR_FILENO);
        std_error.get_value()->close_input();
        std_error.get_value()->close_output();
    }

    if(working_directory.has_value()) {
        boost::filesystem::current_path(working_directory.get_value());
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
        std_input.get_value()->close_input();
    }

    if(std_output.has_value()) {
        std_output.get_value()->close_output();
    }

    if(std_error.has_value()) {
        std_error.get_value()->close_output();
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
        else if(WIFSIGNALED(status)) {
            LOG_ERROR(format("process terminated with signal: %s") %
                      strsignal(WTERMSIG(status)));
            return 128 + WTERMSIG(status);
        }
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
