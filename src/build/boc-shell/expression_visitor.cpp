#include "expression_visitor.hpp"
#include "argument_visitor.hpp"
#include "sexpr/sexpr_helpers.hpp"
#include "program_visitor.hpp"
#include "symbols.hpp"
#include "stack.hpp"
#include "log/log.hpp"
#include "utility/zip.hpp"

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

gorc::sexpr gorc::expression_visitor::visit(call_expression &e) const
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

        std::vector<sexpr> args;
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
        return make_sexpr();
    }
}
