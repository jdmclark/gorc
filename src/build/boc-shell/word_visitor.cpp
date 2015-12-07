#include "word_visitor.hpp"
#include "stack.hpp"
#include "sexpr_helpers.hpp"
#include "system/env.hpp"
#include "log/log.hpp"

std::string gorc::word_visitor::visit(simple_word &w) const
{
    return w.value;
}

std::string gorc::word_visitor::visit(variable_name &var) const
{
    return as_string_value(get_variable_value(var.name));
}

std::string gorc::word_visitor::visit(environment_variable_name &var) const
{
    auto value = get_environment_variable(var.name);
    if(value.has_value()) {
        return value.get_value();
    }
    else {
        LOG_FATAL(format("environment variable '%s' is undefined") % var.name);
    }
}
