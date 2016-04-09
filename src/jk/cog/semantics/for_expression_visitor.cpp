#include "for_expression_visitor.hpp"
#include "rvalue_visitor.hpp"

using namespace gorc;
using namespace gorc::cog;

for_expression_visitor::for_expression_visitor(script &out_script,
                                               ast_factory &factory,
                                               constant_table const &constants,
                                               verb_table const &verbs,
                                               bool result_is_used)
    : out_script(out_script)
    , factory(factory)
    , constants(constants)
    , verbs(verbs)
    , result_is_used(result_is_used)
{
    return;
}

value_type for_expression_visitor::visit(ast::for_expression &e)
{
    rvalue_visitor rvv(out_script, factory, constants, verbs, result_is_used);
    return visit_and_fold(rvv, *e.condition, factory);
}

value_type for_expression_visitor::visit(ast::for_empty_expression &)
{
    return value_type::boolean;
}
