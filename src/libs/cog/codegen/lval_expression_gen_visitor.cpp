#include "lval_expression_gen_visitor.hpp"
#include "rval_expression_gen_visitor.hpp"
#include "log/log.hpp"

using namespace gorc;
using namespace gorc::cog;

lval_expression_gen_visitor::lval_expression_gen_visitor(script &out_script,
                                                         ir_printer &ir,
                                                         verb_table const &verbs,
                                                         constant_table const &constants)
    : out_script(out_script)
    , ir(ir)
    , verbs(verbs)
    , constants(constants)
{
    return;
}

void lval_expression_gen_visitor::visit(ast::node &)
{
    LOG_FATAL("generated assignment for target which is not an l-value");
}

void lval_expression_gen_visitor::visit(ast::identifier_expression &e)
{
    ir.stor(out_script.symbols.get_symbol_index(e.value->value));
}

void lval_expression_gen_visitor::visit(ast::subscript_expression &e)
{
    // Generate code for subscript
    rval_expression_gen_visitor ev(out_script, ir, verbs, constants);

    ir.stori(out_script.symbols.get_symbol_index(e.base->value));
}
