#include "rval_expression_gen_visitor.hpp"
#include "lval_expression_gen_visitor.hpp"
#include "nonval_expression_gen_visitor.hpp"

using namespace gorc;
using namespace gorc::cog;

rval_expression_gen_visitor::rval_expression_gen_visitor(script &out_script,
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

void rval_expression_gen_visitor::visit(ast::immediate_expression &e)
{
    ir.push(e.value);
    return;
}

// LCOV_EXCL_START
//
// Cannot be called - constant expressions should be lowered before this

void rval_expression_gen_visitor::visit(ast::string_literal_expression &)
{
    LOG_FATAL("unlowered string literal expression");
}

void rval_expression_gen_visitor::visit(ast::integer_literal_expression &)
{
    LOG_FATAL("unlowered integer literal expression");
}

void rval_expression_gen_visitor::visit(ast::float_literal_expression &)
{
    LOG_FATAL("unlowered float literal expression");
}

void rval_expression_gen_visitor::visit(ast::vector_literal_expression &)
{
    LOG_FATAL("unlowered vector literal expression");
}

// LCOV_EXCL_STOP

void rval_expression_gen_visitor::visit(ast::identifier_expression &e)
{
    ir.load(out_script.symbols.get_symbol_index(e.value->value));
}

void rval_expression_gen_visitor::visit(ast::subscript_expression &e)
{
    // Push index
    ast_visit(*this, *e.index);

    ir.loadi(out_script.symbols.get_symbol_index(e.base->value));
}

void rval_expression_gen_visitor::visit(ast::method_call_expression &e)
{
    // Push arguments, left to right
    for(auto &arg : e.arguments->elements) {
        ast_visit(*this, *arg);
    }

    ir.callv(verbs.get_verb_id(e.base->value), e.location);
}

void rval_expression_gen_visitor::visit(ast::unary_expression &e)
{
    ast_visit(*this, *e.base);

    switch(e.op) {
    case ast::unary_operator::plus:
        // Non-operation
        break;

    case ast::unary_operator::minus:
        ir.neg();
        break;

    case ast::unary_operator::logical_not:
        ir.lnot();
        break;
    }
}

void rval_expression_gen_visitor::visit(ast::infix_expression &e)
{
    ast_visit(*this, *e.left);
    ast_visit(*this, *e.right);

    switch(e.op) {
    case ast::infix_operator::addition:
        ir.add();
        break;

    case ast::infix_operator::subtraction:
        ir.sub();
        break;

    case ast::infix_operator::multiplication:
        ir.mul();
        break;

    case ast::infix_operator::division:
        ir.div();
        break;

    case ast::infix_operator::modulo:
        ir.mod();
        break;

    case ast::infix_operator::bitwise_or:
        ir.bor();
        break;

    case ast::infix_operator::bitwise_and:
        ir.band();
        break;

    case ast::infix_operator::bitwise_xor:
        ir.bxor();
        break;

    case ast::infix_operator::logical_or:
        ir.lor();
        break;

    case ast::infix_operator::logical_and:
        ir.land();
        break;

    case ast::infix_operator::equal:
        ir.eq();
        break;

    case ast::infix_operator::not_equal:
        ir.ne();
        break;

    case ast::infix_operator::greater:
        ir.gt();
        break;

    case ast::infix_operator::greater_equal:
        ir.ge();
        break;

    case ast::infix_operator::less:
        ir.lt();
        break;

    case ast::infix_operator::less_equal:
        ir.le();
        break;
    }
}

void rval_expression_gen_visitor::visit(ast::assignment_expression &e)
{
    // Push value
    ast_visit(*this, *e.value);

    // Need value on stack after
    ir.dup();

    // Push index and store
    lval_expression_gen_visitor ev(out_script, ir, verbs, constants);
    ast_visit(ev, *e.target);
}

void rval_expression_gen_visitor::visit(ast::comma_expression &e)
{
    nonval_expression_gen_visitor ev(out_script, ir, verbs, constants);

    ast_visit(ev, *e.left);
    ast_visit(*this, *e.right);
}

void rval_expression_gen_visitor::visit(ast::for_empty_expression &)
{
    ir.push(value(true));
}

void rval_expression_gen_visitor::visit(ast::for_expression &e)
{
    ast_visit(*this, *e.condition);
}
