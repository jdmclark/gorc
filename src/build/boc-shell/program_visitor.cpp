#include "program_visitor.hpp"
#include "log/log.hpp"
#include "stack.hpp"
#include "sexpr_helpers.hpp"
#include "system/pipe.hpp"
#include "system/process.hpp"
#include "argument_visitor.hpp"
#include "expression_visitor.hpp"
#include "assignment_visitor.hpp"

int gorc::program_visitor::visit(pipe_command &cmd) const
{
    // Open interprocess pipes
    size_t num_subcommands = cmd.subcommands->elements.size();

    std::vector<std::unique_ptr<pipe>> pipes;
    for(size_t i = 1; i < num_subcommands; ++i) {
        pipes.push_back(make_unique<pipe>());
    }

    std::vector<maybe<pipe*>> stdin_pipes;
    std::vector<maybe<pipe*>> stdout_pipes;

    // First process takes stdin from console
    stdin_pipes.push_back(nothing); // TODO: stdin redirection

    for(auto &pipe : pipes) {
        stdin_pipes.push_back(pipe.get());
        stdout_pipes.push_back(pipe.get());
    }

    // Last process sends stdout to console
    stdout_pipes.push_back(nothing); // TODO: stdout redirection

    auto sub_it = cmd.subcommands->elements.begin();
    auto stdin_it = stdin_pipes.begin();
    auto stdout_it = stdout_pipes.begin();

    std::vector<std::unique_ptr<process>> processes;

    while(sub_it != cmd.subcommands->elements.end() &&
          stdin_it != stdin_pipes.end() &&
          stdout_it != stdout_pipes.end()) {
        auto &sub_args = (*sub_it)->arguments->elements;

        // First token is the program to execute
        std::string prog = ast_visit(argument_visitor(), sub_args.front());
        std::vector<std::string> args;
        for(auto it = sub_args.begin() + 1;
            it != sub_args.end();
            ++it) {
            args.push_back(ast_visit(argument_visitor(), *it));
        }

        processes.push_back(make_unique<process>(prog,
                                                 args,
                                                 *stdin_it,
                                                 *stdout_it,
                                                 /* err */ nothing));
        ++sub_it;
        ++stdin_it;
        ++stdout_it;
    }

    int last_exit_code = 0;
    for(auto &proc : processes) {
        last_exit_code = proc->join();
    }

    return last_exit_code;
}

int gorc::program_visitor::visit(command_statement &cmd) const
{
    return ast_visit(*this, *cmd.cmd);
}

int gorc::program_visitor::visit(assignment_statement &s) const
{
    sexpr value = make_sexpr(ast_visit(argument_visitor(), s.value));

    ast_visit(assign_lvalue_visitor(value), *s.var);

    return EXIT_SUCCESS;
}

int gorc::program_visitor::visit(var_declaration_statement &s) const
{
    sexpr value;
    if(s.value.has_value()) {
        value = make_sexpr(ast_visit(argument_visitor(), s.value.get_value()));
    }

    create_variable(s.var->name, value);

    return EXIT_SUCCESS;
}

int gorc::program_visitor::visit(if_statement &s) const
{
    auto cond = ast_visit(expression_visitor(), *s.condition);
    if(as_boolean_value(cond)) {
        ast_visit(*this, *s.code);
    }

    return EXIT_SUCCESS;
}

int gorc::program_visitor::visit(if_else_statement &s) const
{
    auto cond = ast_visit(expression_visitor(), *s.condition);
    if(as_boolean_value(cond)) {
        ast_visit(*this, *s.code);
    }
    else {
        ast_visit(*this, *s.elsecode);
    }

    return EXIT_SUCCESS;
}

int gorc::program_visitor::visit(ast_list_node<statement*> &stmt_seq) const
{
    for(auto &stmt : stmt_seq.elements) {
        int result = ast_visit(*this, *stmt);
        if(result != 0) {
            // Abort executing a statement sequence after the first error
            LOG_ERROR(format("command failed with code %d") % result);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int gorc::program_visitor::visit(compound_statement &stmt) const
{
    scoped_stack_frame sf;
    return ast_visit(*this, stmt.code);
}

int gorc::program_visitor::visit(translation_unit &tu) const
{
    return ast_visit(*this, tu.code);
}
