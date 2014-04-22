#include "expression_visitor.h"
#include "lval_visitor.h"
#include "base/diagnostics/helper.h"
#include <algorithm>

using namespace gorc::cog::ast;
using gorc::cog::stages::semantic_analysis::expression_visitor;

expression_visitor::expression_visitor(symbols::symbol_table& symbolTable,
    const std::unordered_map<std::string, vm::value>& constantTable,
    verbs::verb_table& verbTable, diagnostics::report& report)
    : ast::visitor("Stage2::ExpressionVisitor", report), symbol_table(symbolTable),
    ConstantTable(constantTable), verb_table(verbTable),    expression_type(vm::type::nothing) {
    return;
}

void expression_visitor::visit_string_literal_expression(string_literal_expression&) {
    expression_type = vm::type::string;
}

void expression_visitor::visit_integer_literal_expression(integer_literal_expression&) {
    expression_type = vm::type::integer;
}

void expression_visitor::visit_float_literal_expression(float_literal_expression&) {
    expression_type = vm::type::floating;
}

void expression_visitor::visit_vector_literal_expression(vector_literal_expression&) {
    expression_type = vm::type::vector;
}

void expression_visitor::visit_identifier_expression(identifier_expression& e) {
    // Convert identifier to lowercase for processing.
    std::transform(e.identifier.begin(), e.identifier.end(), e.identifier.begin(), tolower);

    // Check constant table.
    auto it = ConstantTable.find(e.identifier);
    if(it != ConstantTable.end()) {
        // identifier represents a constant.
        expression_type = it->second.get_type();
    }
    else {
        // identifier does not represent a constant.

        // Actual type is not known, but the result must be
        // an actual value. Fake the result.
        expression_type = vm::type::integer;

        if(!symbol_table.is_symbol_defined(e.identifier)) {
            diagnostics::helper::undefined_symbol(report, visitor_name, e.identifier, e.location);
            symbol_table.add_symbol(e.identifier);
        }
    }
}

void expression_visitor::visit_subscript_expression(subscript_expression& e) {
    // Convert base identifier to lowercase for processing.
    std::transform(e.base.begin(), e.base.end(), e.base.begin(), tolower);

    if(!symbol_table.is_symbol_defined(e.base)) {
        diagnostics::helper::undefined_array_base(report, visitor_name, e.base, e.location);
    }

    expression_visitor subscript(symbol_table, ConstantTable, verb_table, report);
    e.index->accept(subscript);

    if(subscript.expression_type == vm::type::nothing) {
        diagnostics::helper::illegal_void_result(report, visitor_name, e.index->location);
    }

    // Actual type is not known, but the result must be
    // an actual value. Fake the result.
    expression_type = vm::type::integer;
}

void expression_visitor::visit_method_call_expression(method_call_expression& e) {
    // Convert verb name to lowercase for processing.
    std::transform(e.base.begin(), e.base.end(), e.base.begin(), tolower);

    if(!verb_table.is_verb_defined(e.base)) {
        diagnostics::helper::unknown_verb(report, visitor_name, e.base, e.location);
        expression_type = vm::type::integer;
    }
    else {
        auto id = verb_table.get_verb(e.base);
        size_t pcount = verb_table.parameter_count(id);
        expression_type = verb_table.return_type(id);

        if(pcount != e.arguments->size()) {
            diagnostics::helper::invalid_argument_count(report, visitor_name, e.base, pcount, e.arguments->size(), e.location);
        }
        else {
            for(auto& arg : *e.arguments) {
                expression_visitor av(symbol_table, ConstantTable, verb_table, report);
                arg->accept(av);

                if(av.expression_type == vm::type::nothing) {
                    diagnostics::helper::illegal_void_result(report, visitor_name, arg->location);
                }
            }
        }
    }

    return;
}

void expression_visitor::visit_unary_expression(unary_expression& e) {
    expression_visitor v(symbol_table, ConstantTable, verb_table, report);
    e.base->accept(v);

    if(v.expression_type == vm::type::nothing) {
        diagnostics::helper::illegal_void_result(report, visitor_name, e.base->location);
        v.expression_type = vm::type::integer;
    }

    switch(e.op) {
    case unary_operator::plus:
    case unary_operator::minus:
        expression_type = v.expression_type;
        return;

    case unary_operator::logical_not:
        if(v.expression_type != vm::type::boolean) {
            diagnostics::helper::result_not_boolean(report, visitor_name, e.location);
        }
        
        expression_type = vm::type::boolean;
        return;
    }
}

void expression_visitor::visit_infix_expression(infix_expression& e) {
    expression_visitor left(symbol_table, ConstantTable, verb_table, report);
    e.left->accept(left);

    expression_visitor right(symbol_table, ConstantTable, verb_table, report);
    e.right->accept(right);

    // Left and right must both have values.
    if(left.expression_type == vm::type::nothing) {
        diagnostics::helper::illegal_void_result(report, visitor_name, e.left->location);
        left.expression_type = vm::type::integer;
    }

    if(right.expression_type == vm::type::nothing) {
        diagnostics::helper::illegal_void_result(report, visitor_name, e.right->location);
        right.expression_type = vm::type::integer;
    }

    switch(e.op) {
    case infix_operator::addition:
    case infix_operator::subtraction:
    case infix_operator::multiplication:
    case infix_operator::division:
    case infix_operator::modulo:
        // Result must have a numeric type.
        // Fake the result.
        expression_type = vm::type::integer;
        break;

    case infix_operator::greater:
    case infix_operator::greater_equal:
    case infix_operator::less:
    case infix_operator::less_equal:
    case infix_operator::equal:
    case infix_operator::not_equal:
        // Result must have a boolean type.
        expression_type = vm::type::boolean;
        break;

    case infix_operator::bitwise_and:
    case infix_operator::bitwise_or:
    case infix_operator::bitwise_xor:
        // Result must have an integer type.
        expression_type = vm::type::integer;
        break;

    case infix_operator::logical_and:
    case infix_operator::logical_or:
        // Inputs must have boolean types.
        if(left.expression_type != vm::type::boolean) {
            diagnostics::helper::result_not_boolean(report, visitor_name, e.left->location);
        }

        if(right.expression_type != vm::type::boolean) {
            diagnostics::helper::result_not_boolean(report, visitor_name, e.right->location);
        }

        // Result must have a boolean type.
        expression_type = vm::type::boolean;
        break;
    }

    return;
}

void expression_visitor::visit_assignment_expression(assignment_expression& e) {
    lvalue_visitor target(symbol_table, ConstantTable, verb_table, report);
    e.target->accept(target);

    expression_visitor value(symbol_table, ConstantTable, verb_table, report);
    e.value->accept(value);

    if(value.expression_type == vm::type::nothing) {
        diagnostics::helper::illegal_void_result(report, visitor_name, e.value->location);
    }

    expression_type = value.expression_type;
}

void expression_visitor::visit_comma_expression(comma_expression& e) {
    expression_visitor left(symbol_table, ConstantTable, verb_table, report);
    e.left->accept(left);

    expression_visitor right(symbol_table, ConstantTable, verb_table, report);
    e.right->accept(right);

    expression_type = right.expression_type;
}

void expression_visitor::visit_for_optional_expression(for_optional_expression&) {
    expression_type = vm::type::boolean;
}

void expression_visitor::visit_for_expression(for_expression& e) {
    e.condition->accept(*this);
    return;
}
