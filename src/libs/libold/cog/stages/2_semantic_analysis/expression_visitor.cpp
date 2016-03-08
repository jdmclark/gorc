#include "expression_visitor.hpp"
#include "lval_visitor.hpp"
#include <algorithm>

using namespace gorc::cog::ast;
using gorc::cog::stages::semantic_analysis::expression_visitor;

expression_visitor::expression_visitor(symbols::symbol_table& symbolTable,
    const std::unordered_map<std::string, vm::value>& constantTable,
    verbs::verb_table& verbTable)
    : symbol_table(symbolTable),
    ConstantTable(constantTable), verb_table(verbTable),    expression_type(vm::type::nothing) {
    return;
}

void expression_visitor::visit(string_literal_expression &e) {
    expression_type = vm::type::string;
    e.stored_string = symbol_table.add_string(e.value);
}

void expression_visitor::visit(integer_literal_expression&) {
    expression_type = vm::type::integer;
}

void expression_visitor::visit(float_literal_expression&) {
    expression_type = vm::type::floating;
}

void expression_visitor::visit(vector_literal_expression&) {
    expression_type = vm::type::vector;
}

void expression_visitor::visit(identifier_expression& e) {
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
            LOG_WARNING(format("symbol '%s' used without being defined") % e.identifier);
            symbol_table.add_symbol(e.identifier);
        }
    }
}

void expression_visitor::visit(subscript_expression& e) {
    // Convert base identifier to lowercase for processing.
    std::transform(e.base.begin(), e.base.end(), e.base.begin(), tolower);

    if(!symbol_table.is_symbol_defined(e.base)) {
        LOG_ERROR(format("undefined array base '%s'") % e.base);
    }

    expression_visitor subscript(symbol_table, ConstantTable, verb_table);
    e.index->accept(subscript);

    if(subscript.expression_type == vm::type::nothing) {
        diagnostic_context dc(*e.index);
        LOG_ERROR("value expected, but expression has result type 'void'");
    }

    // Actual type is not known, but the result must be
    // an actual value. Fake the result.
    expression_type = vm::type::integer;
}

void expression_visitor::visit(method_call_expression& e) {
    // Convert verb name to lowercase for processing.
    std::transform(e.base.begin(), e.base.end(), e.base.begin(), tolower);

    if(!verb_table.is_verb_defined(e.base)) {
        LOG_ERROR(format("unknown verb '%s'") % e.base);
        expression_type = vm::type::integer;
    }
    else {
        auto id = verb_table.get_verb(e.base);
        size_t pcount = verb_table.parameter_count(id);
        expression_type = verb_table.return_type(id);

        if(pcount != e.arguments->size()) {
            LOG_ERROR(format("verb '%s' expects %d arguments, found %d") % e.base % pcount % e.arguments->size());
        }
        else {
            for(auto& arg : *e.arguments) {
                expression_visitor av(symbol_table, ConstantTable, verb_table);
                arg->accept(av);

                if(av.expression_type == vm::type::nothing) {
                    diagnostic_context dc(*arg);
                    LOG_ERROR("value expected, but expression has result type 'void'");
                }
            }
        }
    }

    return;
}

void expression_visitor::visit(unary_expression& e) {
    expression_visitor v(symbol_table, ConstantTable, verb_table);
    e.base->accept(v);

    if(v.expression_type == vm::type::nothing) {
        diagnostic_context dc(*e.base);
        LOG_ERROR("value expected, but expression has result type 'void'");
        v.expression_type = vm::type::integer;
    }

    switch(e.op) {
    case unary_operator::plus:
    case unary_operator::minus:
        expression_type = v.expression_type;
        return;

    case unary_operator::logical_not:
        if(v.expression_type != vm::type::boolean) {
            diagnostic_context dc(*e.base);
            LOG_WARNING("implicit cast to bool used in condition");
        }

        expression_type = vm::type::boolean;
        return;
    }
}

void expression_visitor::visit(infix_expression& e) {
    expression_visitor left(symbol_table, ConstantTable, verb_table);
    e.left->accept(left);

    expression_visitor right(symbol_table, ConstantTable, verb_table);
    e.right->accept(right);

    // Left and right must both have values.
    if(left.expression_type == vm::type::nothing) {
        diagnostic_context dc(*e.left);
        LOG_ERROR("value expected, but expression has result type 'void'");
        left.expression_type = vm::type::integer;
    }

    if(right.expression_type == vm::type::nothing) {
        diagnostic_context dc(*e.right);
        LOG_ERROR("value expected, but expression has result type 'void'");
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
            diagnostic_context dc(*e.left);
            LOG_WARNING("implicit cast to bool used in condition");
        }

        if(right.expression_type != vm::type::boolean) {
            diagnostic_context dc(*e.right);
            LOG_WARNING("implicit cast to bool used in condition");
        }

        // Result must have a boolean type.
        expression_type = vm::type::boolean;
        break;
    }

    return;
}

void expression_visitor::visit(assignment_expression& e) {
    lvalue_visitor target(symbol_table, ConstantTable, verb_table);
    e.target->accept(target);

    expression_visitor value(symbol_table, ConstantTable, verb_table);
    e.value->accept(value);

    if(value.expression_type == vm::type::nothing) {
        diagnostic_context dc(*e.value);
        LOG_ERROR("value expected, but expression has result type 'void'");
    }

    expression_type = value.expression_type;
}

void expression_visitor::visit(comma_expression& e) {
    expression_visitor left(symbol_table, ConstantTable, verb_table);
    e.left->accept(left);

    expression_visitor right(symbol_table, ConstantTable, verb_table);
    e.right->accept(right);

    expression_type = right.expression_type;
}

void expression_visitor::visit(for_empty_expression&) {
    expression_type = vm::type::boolean;
}

void expression_visitor::visit(for_expression& e) {
    e.condition->accept(*this);
    return;
}
