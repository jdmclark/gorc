#include "lvalue_visitor.hpp"
#include "rvalue_visitor.hpp"
#include "ast/variant_location_visitor.hpp"

using namespace gorc;
using namespace gorc::cog;

lvalue_visitor::lvalue_visitor(script &out_script,
                               ast_factory &factory,
                               constant_table const &constants,
                               verb_table const &verbs)
    : out_script(out_script)
    , factory(factory)
    , constants(constants)
    , verbs(verbs)
{
    return;
}

value_type lvalue_visitor::visit(ast_node &)
{
    LOG_ERROR("assignment target is not an lvalue");
    return value_type::dynamic;
}

value_type lvalue_visitor::visit(ast::identifier_expression &e)
{
    auto const &sym = out_script.symbols.get_symbol(e.value->value);
    return sym.type;
}

value_type lvalue_visitor::visit(ast::subscript_expression &e)
{
    visit_and_fold_array_subscript(*e.index,
                                   out_script,
                                   factory,
                                   constants,
                                   verbs);

    // Look up base symbol, and assume indexed symbol shares type
    auto const &sym = out_script.symbols.get_symbol(e.base->value);
    return sym.type;
}
