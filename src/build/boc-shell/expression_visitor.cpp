#include "expression_visitor.hpp"
#include "argument_visitor.hpp"
#include "program_visitor.hpp"
#include "symbols.hpp"
#include "stack.hpp"
#include "log/log.hpp"
#include "utility/zip.hpp"

gorc::shvalue gorc::expression_visitor::visit(argument_expression &e) const
{
    return ast_visit(argument_visitor(), e.value);
}

gorc::shvalue gorc::expression_visitor::visit(unary_expression &e) const
{
    auto sub_value = ast_visit(expression_visitor(), *e.value);

    switch(e.op) {
    case unary_operator::logical_not:
        return shvalue_from_boolean(!shvalue_to_boolean(sub_value));

// LCOV_EXCL_START
    }

    // Not coverable
    LOG_FATAL("unhandled unary operator");

// LCOV_EXCL_STOP
}

gorc::shvalue gorc::expression_visitor::visit(infix_expression &e) const
{
    auto left_value = ast_visit(expression_visitor(), *e.left);
    auto right_value = ast_visit(expression_visitor(), *e.right);

    switch(e.op) {
    case infix_operator::equal:
        return shvalue_from_boolean(left_value == right_value);

    case infix_operator::not_equal:
        return shvalue_from_boolean(left_value != right_value);

    case infix_operator::cons: {
            shvalue rv = left_value;
            std::copy(right_value.begin(), right_value.end(), std::back_inserter(rv));
            return rv;
        }

// LCOV_EXCL_START
    }

    // Not coverable
    LOG_FATAL("unhandled infix operator");

// LCOV_EXCL_STOP
}

gorc::shvalue gorc::expression_visitor::visit(nil_expression &) const
{
    return shvalue();
}

gorc::shvalue gorc::expression_visitor::visit(call_expression &e) const
{
    // Try for a builtin first
    auto builtin_ptr = get_builtin(e.name->value);
    if(builtin_ptr.has_value()) {
        auto &builtin = *builtin_ptr.get_value();

        if(builtin.args != e.arguments->elements.size()) {
            LOG_FATAL(format("builtin '%s' expected %d arguments, found %d") %
                      e.name->value %
                      builtin.args %
                      e.arguments->elements.size());
        }

        std::vector<shvalue> args;
        for(auto const &arg : e.arguments->elements) {
            args.push_back(ast_visit(expression_visitor(), *arg));
        }

        return builtin.code(args);
    }

    auto &fn_node = get_function(e.name->value);

    if(fn_node.arguments->elements.size() != e.arguments->elements.size()) {
        LOG_FATAL(format("function '%s' expected %d arguments, found %d") %
                  e.name->value %
                  fn_node.arguments->elements.size() %
                  e.arguments->elements.size());
    }

    scoped_stack_frame fn_stack;

    for(auto arg : zip(fn_node.arguments->elements, e.arguments->elements)) {
        create_variable(std::get<0>(arg)->value,
                        ast_visit(expression_visitor(), *std::get<1>(arg)));
    }

    program_visitor pv;
    ast_visit(pv, *fn_node.code);

    if(pv.return_value.has_value()) {
        return pv.return_value.get_value();
    }
    else {
        return shvalue();
    }
}
