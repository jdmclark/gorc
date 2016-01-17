#include "command_visitor.hpp"
#include "system/process.hpp"
#include "system/pipe.hpp"
#include "io_redirection_visitor.hpp"
#include "log/log.hpp"
#include "argument_visitor.hpp"
#include <memory>

gorc::command_visitor::command_visitor(maybe<shvalue*> stdout_variable_target)
    : stdout_variable_target(stdout_variable_target)
{
    return;
}

int gorc::command_visitor::visit(pipe_command &cmd)
{
    // Open redirection pipes
    std::unique_ptr<gorc::pipe> stored_redirected_stdin_pipe;
    if(cmd.stdin_source.has_value()) {
        stored_redirected_stdin_pipe = ast_visit(io_redirection_visitor(/* input */ true),
                                                 cmd.stdin_source.get_value());
    }
    else {
        // Use a normal pipe, but close the output end.
        // This will cause the child process to exit with SIGPIPE instead of halting.
        stored_redirected_stdin_pipe = std::make_unique<gorc::pipe>();
        stored_redirected_stdin_pipe->close_output();
    }

    std::unique_ptr<gorc::pipe> stored_redirected_stdout_pipe;
    maybe<gorc::pipe*> redirected_stdout_pipe;
    if(cmd.stdout_target.has_value()) {
        stored_redirected_stdout_pipe = ast_visit(io_redirection_visitor(/* input */ false),
                                                  cmd.stdout_target.get_value());
        redirected_stdout_pipe = stored_redirected_stdout_pipe.get();
    }
    else if(stdout_variable_target.has_value()) {
        // stdout is being redirected to a shvalue
        stored_redirected_stdout_pipe = std::make_unique<gorc::pipe>();
        redirected_stdout_pipe = stored_redirected_stdout_pipe.get();
    }

    std::unique_ptr<gorc::pipe> stored_redirected_stderr_pipe;
    maybe<gorc::pipe*> redirected_stderr_pipe;
    if(cmd.stderr_target.has_value()) {
        stored_redirected_stderr_pipe = ast_visit(io_redirection_visitor(/* input */ false),
                                                  cmd.stderr_target.get_value());
        stored_redirected_stderr_pipe->set_reusable(true);
        redirected_stderr_pipe = stored_redirected_stderr_pipe.get();
    }

    // Open interprocess pipes
    size_t num_subcommands = cmd.subcommands->elements.size();

    std::vector<std::unique_ptr<gorc::pipe>> pipes;
    for(size_t i = 1; i < num_subcommands; ++i) {
        pipes.push_back(std::make_unique<pipe>());
    }

    std::vector<maybe<gorc::pipe*>> stdin_pipes;
    std::vector<maybe<gorc::pipe*>> stdout_pipes;

    // First process takes stdin from redirection
    stdin_pipes.push_back(stored_redirected_stdin_pipe.get());

    for(auto &pipe : pipes) {
        stdin_pipes.push_back(pipe.get());
        stdout_pipes.push_back(pipe.get());
    }

    // Last process sends stdout to console
    stdout_pipes.push_back(redirected_stdout_pipe);

    auto sub_it = cmd.subcommands->elements.begin();
    auto stdin_it = stdin_pipes.begin();
    auto stdout_it = stdout_pipes.begin();

    std::vector<std::unique_ptr<process>> processes;

    while(sub_it != cmd.subcommands->elements.end() &&
          stdin_it != stdin_pipes.end() &&
          stdout_it != stdout_pipes.end()) {
        shvalue argv = ast_visit(argument_visitor(), (*sub_it)->arguments);

        if(argv.empty()) {
            LOG_FATAL("cannot execute empty command");
        }

        // First token is the program to execute
        std::string prog = argv.front();
        std::vector<std::string> args;
        for(auto it = argv.begin() + 1;
            it != argv.end();
            ++it) {
            args.push_back(*it);
        }

        processes.push_back(std::make_unique<process>(prog,
                                                      args,
                                                      *stdin_it,
                                                      *stdout_it,
                                                      redirected_stderr_pipe));
        ++sub_it;
        ++stdin_it;
        ++stdout_it;
    }

    if(stdout_variable_target.has_value()) {
        // Read lines from stdout until EOF
        auto &stdout_var = *stdout_variable_target.get_value();
        auto &stdout_pipe = redirected_stdout_pipe.get_value()->get_input();

        char buffer[3000];
        std::string line_buffer;

        while(true) {
            auto amt = stdout_pipe.read_some(buffer, 3000);
            for(size_t i = 0; i < amt; ++i) {
                if(buffer[i] == '\n') {
                    if(!line_buffer.empty()) {
                        stdout_var.push_back(line_buffer);
                        line_buffer.clear();
                    }
                }
                else {
                    line_buffer.push_back(buffer[i]);
                }
            }

            if(stdout_pipe.at_end()) {
                break;
            }
        }

        if(!line_buffer.empty()) {
            stdout_var.push_back(line_buffer);
        }
    }

    int last_exit_code = 0;
    for(auto &proc : processes) {
        last_exit_code = proc->join();
        exit_code_sequence.push_back(std::to_string(last_exit_code));
    }

    return last_exit_code;
}

int gorc::command_visitor::visit(infix_command &cmd)
{
    int left_return_code = ast_visit(*this, *cmd.left);

    switch(cmd.op) {
    case command_infix_operator::logical_or:
        if(left_return_code == 0) {
            return left_return_code;
        }
        break;

    case command_infix_operator::logical_and:
        if(left_return_code != 0) {
            return left_return_code;
        }
        break;
    }

    return ast_visit(*this, *cmd.right);
}

