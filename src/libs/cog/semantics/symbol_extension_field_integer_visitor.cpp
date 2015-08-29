#include "symbol_extension_field_integer_visitor.hpp"
#include "log/log.hpp"

using gorc::cog::symbol_extension_field_integer_visitor;

int symbol_extension_field_integer_visitor::visit(ast::node &) const
{
    LOG_ERROR(format("extension '%s' value type mismatch") % name);
    return 0;
}

int symbol_extension_field_integer_visitor::visit(ast::integer_field &f) const
{
    return f.value;
}
