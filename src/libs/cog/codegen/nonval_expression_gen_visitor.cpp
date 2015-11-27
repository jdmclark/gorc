#include "nonval_expression_gen_visitor.hpp"
#include "rval_expression_gen_visitor.hpp"
#include "lval_expression_gen_visitor.hpp"

using namespace gorc;
using namespace gorc::cog;

nonval_expression_gen_visitor::nonval_expression_gen_visitor(script &out_script,
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

void nonval_expression_gen_visitor::visit(ast::immediate_expression &)
{
    // Skip
    return;
}

// LCOV_EXCL_START
//
// Literal expressions are folded out. These are normally impossible. Assert.

void nonval_expression_gen_visitor::visit(ast::string_literal_expression &)
{
    LOG_FATAL("unhandled string literal expression in code generation");
}

void nonval_expression_gen_visitor::visit(ast::integer_literal_expression &)
{
    LOG_FATAL("unhandled integer literal expression in code generation");
}

void nonval_expression_gen_visitor::visit(ast::float_literal_expression &)
{
    LOG_FATAL("unhandled float literal expression in code generation");
}

void nonval_expression_gen_visitor::visit(ast::vector_literal_expression &)
{
    LOG_FATAL("unhandled vector literal expression in code generation");
}

// LCOV_EXCL_STOP

void nonval_expression_gen_visitor::visit(ast::identifier_expression &)
{
    // Skip
    return;
}

void nonval_expression_gen_visitor::visit(ast::subscript_expression &e)
{
    ast_visit(*this, *e.index);
}

void nonval_expression_gen_visitor::visit(ast::method_call_expression &e)
{
    rval_expression_gen_visitor rv(out_script, ir, verbs, constants);

    // Push values from left to right
    for(auto &arg : e.arguments->elements) {
        ast_visit(rv, *arg);
    }

    ir.call(verbs.get_verb_id(e.base->value));
}

void nonval_expression_gen_visitor::visit(ast::unary_expression &e)
{
    ast_visit(*this, *e.base);
}

void nonval_expression_gen_visitor::visit(ast::infix_expression &e)
{
    ast_visit(*this, *e.left);
    ast_visit(*this, *e.right);
}

void nonval_expression_gen_visitor::visit(ast::assignment_expression &e)
{
    // Push value
    rval_expression_gen_visitor ev(out_script, ir, verbs, constants);
    ast_visit(ev, *e.value);

    // Push index and store
    lval_expression_gen_visitor lev(out_script, ir, verbs, constants);
    ast_visit(lev, *e.target);
}

void nonval_expression_gen_visitor::visit(ast::comma_expression &e)
{
    ast_visit(*this, *e.left);
    ast_visit(*this, *e.right);
}

void nonval_expression_gen_visitor::visit(ast::for_empty_expression &)
{
    // For empty expression is 'true' constant
    return;
}

void nonval_expression_gen_visitor::visit(ast::for_expression &e)
{
    ast_visit(*this, *e.condition);
}
