#include "word_visitor.hpp"
#include "stack.hpp"
#include "sexpr/sexpr_helpers.hpp"
#include "system/env.hpp"
#include "expression_visitor.hpp"
#include "log/log.hpp"

gorc::sexpr gorc::word_visitor::visit(simple_word &w) const
{
    return make_sexpr(w.value);
}

gorc::sexpr gorc::word_visitor::visit(variable_name &var) const
{
    return get_variable_value(var.name);
}

gorc::sexpr gorc::word_visitor::visit(environment_variable_name &var) const
{
    auto value = get_environment_variable(var.name);
    if(value.has_value()) {
        return make_sexpr(value.get_value());
    }
    else {
        LOG_FATAL(format("environment variable '%s' is undefined") % var.name);
    }
}

gorc::sexpr gorc::word_visitor::visit(expression_word &w) const
{
    return ast_visit(expression_visitor(), *w.value);
}
