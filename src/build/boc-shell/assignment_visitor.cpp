#include "assignment_visitor.hpp"
#include "system/env.hpp"
#include "stack.hpp"

gorc::assign_lvalue_visitor::assign_lvalue_visitor(shvalue const &value)
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
    set_environment_variable(var.name, shvalue_to_string(value));
}
