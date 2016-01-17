#include "word_visitor.hpp"
#include "stack.hpp"
#include "system/env.hpp"
#include "expression_visitor.hpp"
#include "log/log.hpp"
#include "command_visitor.hpp"

gorc::shvalue gorc::word_visitor::visit(simple_word &w) const
{
    return shvalue_from_string(w.value);
}

gorc::shvalue gorc::word_visitor::visit(variable_name &var) const
{
    return get_variable_value(var.name);
}

gorc::shvalue gorc::word_visitor::visit(environment_variable_name &var) const
{
    auto value = get_environment_variable(var.name);
    if(value.has_value()) {
        return shvalue_from_string(value.get_value());
    }
    else {
        LOG_FATAL(format("environment variable '%s' is undefined") % var.name);
    }
}

gorc::shvalue gorc::word_visitor::visit(expression_word &w) const
{
    return ast_visit(expression_visitor(), *w.value);
}

gorc::shvalue gorc::word_visitor::visit(subshell_word &w) const
{
    shvalue rv;

    command_visitor cv(&rv);
    int return_code = ast_visit(cv, *w.cmd);
    set_variable_value("?", cv.exit_code_sequence);

    if(return_code != 0) {
        LOG_FATAL(format("command failed with code %d") % return_code);
    }

    return rv;
}
