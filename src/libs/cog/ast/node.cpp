#include "node.hpp"

gorc::cog::ast::node::node(diagnostic_context_location const &location)
    : location(location)
{
    return;
}

gorc::cog::ast::node::~node()
{
    return;
}
