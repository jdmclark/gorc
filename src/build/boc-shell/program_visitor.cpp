#include "program_visitor.hpp"
#include "log/log.hpp"
#include "stack.hpp"
#include "system/pipe.hpp"
#include "system/process.hpp"
#include "argument_visitor.hpp"
#include "expression_visitor.hpp"
#include "assignment_visitor.hpp"
#include "io_redirection_visitor.hpp"
#include "io/path.hpp"
#include <boost/filesystem.hpp>
#include <stack>
#include <memory>

int gorc::program_visitor::visit(pipe_command &cmd)
{
    // Open redirection pipes
    std::unique_ptr<pipe> stored_redirected_stdin_pipe;
    if(cmd.stdin_source.has_value()) {
        stored_redirected_stdin_pipe = ast_visit(io_redirection_visitor(/* input */ true),
                                                 cmd.stdin_source.get_value());
    }
    else {
        // Use a normal pipe, but close the output end.
        // This will cause the child process to exit with SIGPIPE instead of halting.
        stored_redirected_stdin_pipe = std::make_unique<pipe>();
        stored_redirected_stdin_pipe->close_output();
    }

    std::unique_ptr<pipe> stored_redirected_stdout_pipe;
    maybe<pipe*> redirected_stdout_pipe;
    if(cmd.stdout_target.has_value()) {
        stored_redirected_stdout_pipe = ast_visit(io_redirection_visitor(/* input */ false),
                                                  cmd.stdout_target.get_value());
        redirected_stdout_pipe = stored_redirected_stdout_pipe.get();
    }

    std::unique_ptr<pipe> stored_redirected_stderr_pipe;
    maybe<pipe*> redirected_stderr_pipe;
    if(cmd.stderr_target.has_value()) {
        stored_redirected_stderr_pipe = ast_visit(io_redirection_visitor(/* input */ false),
                                                  cmd.stderr_target.get_value());
        stored_redirected_stderr_pipe->set_reusable(true);
        redirected_stderr_pipe = stored_redirected_stderr_pipe.get();
    }

    // Open interprocess pipes
    size_t num_subcommands = cmd.subcommands->elements.size();

    std::vector<std::unique_ptr<pipe>> pipes;
    for(size_t i = 1; i < num_subcommands; ++i) {
        pipes.push_back(std::make_unique<pipe>());
    }

    std::vector<maybe<pipe*>> stdin_pipes;
    std::vector<maybe<pipe*>> stdout_pipes;

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

    int last_exit_code = 0;
    for(auto &proc : processes) {
        last_exit_code = proc->join();
        exit_code_sequence.push_back(std::to_string(last_exit_code));
    }

    return last_exit_code;
}

int gorc::program_visitor::visit(infix_command &cmd)
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

void gorc::program_visitor::visit(command_statement &cmd)
{
    exit_code_sequence.clear();

    int return_code = ast_visit(*this, *cmd.cmd);
    set_variable_value("?", exit_code_sequence);

    if(return_code != 0) {
        LOG_FATAL(format("command failed with code %d") % return_code);
    }
}

void gorc::program_visitor::visit(assignment_statement &s)
{
    shvalue value = ast_visit(argument_visitor(), s.value);

    ast_visit(assign_lvalue_visitor(value), *s.var);
}

void gorc::program_visitor::visit(var_declaration_statement &s)
{
    shvalue value;
    if(s.value.has_value()) {
        value = ast_visit(argument_visitor(), s.value.get_value());
    }

    create_variable(s.var->name, value);
}

void gorc::program_visitor::visit(if_statement &s)
{
    auto cond = ast_visit(expression_visitor(), *s.condition);
    if(shvalue_to_boolean(cond)) {
        ast_visit(*this, *s.code);
    }
}

void gorc::program_visitor::visit(if_else_statement &s)
{
    auto cond = ast_visit(expression_visitor(), *s.condition);
    if(shvalue_to_boolean(cond)) {
        ast_visit(*this, *s.code);
    }
    else {
        ast_visit(*this, *s.elsecode);
    }
}

void gorc::program_visitor::visit(for_statement &s)
{
    auto list = ast_visit(expression_visitor(), *s.list);
    for(auto const &em : list) {
        scoped_stack_frame sf;
        create_variable(s.varname->value, shvalue_from_string(em));
        ast_visit(*this, *s.code);
    }
}

void gorc::program_visitor::visit(return_statement &s)
{
    if(s.value.has_value()) {
        return_value = ast_visit(expression_visitor(), *s.value.get_value());
    }
    else {
        return_value = shvalue();
    }
}

void gorc::program_visitor::visit(call_statement &s)
{
    ast_visit(expression_visitor(), *s.value);
}

namespace {
    std::stack<gorc::path> working_directory_stack;
}

void gorc::program_visitor::visit(pushd_statement &s)
{
    // Get new directory
    auto arg_value = ast_visit(argument_visitor(), s.new_dir);
    if(arg_value.size() != 1) {
        LOG_FATAL(format("expected working directory path, found '%s' path fragments") %
                  arg_value.size());
    }

    // Store current working directory
    working_directory_stack.push(boost::filesystem::absolute(boost::filesystem::current_path()));
    boost::filesystem::current_path(boost::filesystem::absolute(arg_value.at(0)));
}

void gorc::program_visitor::visit(popd_statement &)
{
    if(working_directory_stack.empty()) {
        LOG_FATAL("cannot popd, working directory stack is empty");
    }

    boost::filesystem::current_path(working_directory_stack.top());
    working_directory_stack.pop();
}

void gorc::program_visitor::visit(ast_list_node<statement*> &stmt_seq)
{
    for(auto &stmt : stmt_seq.elements) {
        // Abort executing a statement sequence after the first return statement
        if(return_value.has_value()) {
            return;
        }

        ast_visit(*this, *stmt);
    }
}

void gorc::program_visitor::visit(include_statement &stmt)
{
    return ast_visit(*this, stmt.code);
}

void gorc::program_visitor::visit(compound_statement &stmt)
{
    scoped_stack_frame sf;
    return ast_visit(*this, stmt.code);
}

void gorc::program_visitor::visit(translation_unit &tu)
{
    return ast_visit(*this, tu.code);
}

void gorc::program_visitor::visit(func_declaration_statement &)
{
    // Ignore function nodes during execution
}
