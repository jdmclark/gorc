#include "symbol_extension_field_string_visitor.hpp"
#include "log/log.hpp"

using gorc::cog::symbol_extension_field_string_visitor;

std::string symbol_extension_field_string_visitor::visit(ast::string_fragment_field &f) const
{
    return f.value;
}

std::string symbol_extension_field_string_visitor::visit(ast::integer_field &f) const
{
    return str(format("%d") % f.value);
}

std::string symbol_extension_field_string_visitor::visit(ast::float_field &f) const
{
    return str(format("%f") % f.value);
}
