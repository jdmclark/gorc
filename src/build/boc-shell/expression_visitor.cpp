#include "expression_visitor.hpp"
#include "argument_visitor.hpp"
#include "sexpr/sexpr_helpers.hpp"
#include "log/log.hpp"

gorc::sexpr gorc::expression_visitor::visit(argument_expression &e) const
{
    return ast_visit(argument_visitor(), e.value);
}

gorc::sexpr gorc::expression_visitor::visit(unary_expression &e) const
{
    auto sub_value = ast_visit(expression_visitor(), *e.value);

    switch(e.op) {
    case unary_operator::logical_not:
        return make_sexpr(!as_boolean_value(sub_value));

    case unary_operator::car:
        return car(sub_value);

    case unary_operator::cdr:
        return cdr(sub_value);

    case unary_operator::atom:
        return make_sexpr(atom(sub_value));

    case unary_operator::null:
        return make_sexpr(null(sub_value));

// LCOV_EXCL_START
    }

    // Not coverable
    LOG_FATAL("unhandled unary operator");

// LCOV_EXCL_STOP
}

gorc::sexpr gorc::expression_visitor::visit(infix_expression &e) const
{
    auto left_value = ast_visit(expression_visitor(), *e.left);
    auto right_value = ast_visit(expression_visitor(), *e.right);

    switch(e.op) {
    case infix_operator::equal:
        return make_sexpr(sexpr_equal(left_value, right_value));

    case infix_operator::not_equal:
        return make_sexpr(!sexpr_equal(left_value, right_value));

    case infix_operator::cons:
        return make_sexpr(left_value, right_value);

// LCOV_EXCL_START
    }

    // Not coverable
    LOG_FATAL("unhandled infix operator");

// LCOV_EXCL_STOP
}

gorc::sexpr gorc::expression_visitor::visit(nil_expression &) const
{
    return make_sexpr();
}
