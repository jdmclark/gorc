#include "expression_visitor.hpp"
#include "argument_visitor.hpp"
#include "log/log.hpp"

std::string gorc::expression_visitor::visit(argument_expression &e) const
{
    return ast_visit(argument_visitor(), e.value);
}

std::string gorc::expression_visitor::visit(unary_expression &e) const
{
    auto sub_value = ast_visit(expression_visitor(), *e.value);

    switch(e.op) {
    case unary_operator::logical_not:
        if(sub_value == "true") {
            return "false";
        }
        else {
            return "true";
        }

// LCOV_EXCL_START
    }

    // Not coverable
    LOG_FATAL("unhandled unary operator");

// LCOV_EXCL_STOP
}

std::string gorc::expression_visitor::visit(infix_expression &e) const
{
    auto left_value = ast_visit(expression_visitor(), *e.left);
    auto right_value = ast_visit(expression_visitor(), *e.right);

    switch(e.op) {
    case infix_operator::equal:
        if(left_value == right_value) {
            return "true";
        }
        else {
            return "false";
        }

    case infix_operator::not_equal:
        if(left_value != right_value) {
            return "true";
        }
        else {
            return "false";
        }
// LCOV_EXCL_START
    }

    // Not coverable
    LOG_FATAL("unhandled infix operator");

// LCOV_EXCL_STOP
}
