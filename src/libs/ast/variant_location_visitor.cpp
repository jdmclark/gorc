#include "variant_location_visitor.hpp"

gorc::diagnostic_context_location gorc::variant_location_visitor::visit(ast_node &n) const
{
    return n.location;
}
