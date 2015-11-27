#include "node.hpp"

gorc::ast_node::ast_node(diagnostic_context_location const &location)
    : location(location)
{
    return;
}

gorc::ast_node::~ast_node()
{
    return;
}
