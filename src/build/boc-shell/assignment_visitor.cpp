#include "assignment_visitor.hpp"
#include "sexpr_helpers.hpp"
#include "system/env.hpp"
#include "stack.hpp"

gorc::assign_lvalue_visitor::assign_lvalue_visitor(sexpr value)
    : value(value)
{
    return;
}

void gorc::assign_lvalue_visitor::visit(variable_name &var) const
{
    set_variable_value(var.name, value);
}

void gorc::assign_lvalue_visitor::visit(environment_variable_name &var) const
{
    set_environment_variable(var.name, as_string_value(value));
}
