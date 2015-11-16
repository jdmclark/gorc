#include "variant_location_visitor.hpp"

gorc::diagnostic_context_location gorc::cog::variant_location_visitor::visit(ast::node &n) const
{
    return n.location;
}
