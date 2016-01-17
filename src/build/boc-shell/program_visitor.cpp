#include "program_visitor.hpp"
#include "log/log.hpp"
#include "stack.hpp"
#include "argument_visitor.hpp"
#include "expression_visitor.hpp"
#include "assignment_visitor.hpp"
#include "command_visitor.hpp"
#include "io/path.hpp"
#include <boost/filesystem.hpp>
#include <stack>
#include <memory>

void gorc::program_visitor::visit(command_statement &cmd)
{
    command_visitor cv;

    int return_code = ast_visit(cv, *cmd.cmd);
    set_variable_value("?", cv.exit_code_sequence);

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
