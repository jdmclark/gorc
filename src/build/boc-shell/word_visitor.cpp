#include "word_visitor.hpp"
#include "stack.hpp"
#include "system/env.hpp"
#include "expression_visitor.hpp"
#include "log/log.hpp"

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
