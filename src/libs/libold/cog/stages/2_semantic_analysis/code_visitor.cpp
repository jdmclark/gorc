#include "code_visitor.hpp"
#include "expression_visitor.hpp"

#include <algorithm>

using namespace gorc::cog::ast;
using gorc::cog::stages::semantic_analysis::code_visitor;

code_visitor::code_visitor(symbols::symbol_table& symbolTable,
    const std::unordered_map<std::string, vm::value>& constantTable,
    verbs::verb_table& verbTable, const std::unordered_set<std::string>& SeenLabels,
    bool inside_loop, bool inside_block)
    : symbol_table(symbolTable),
    ConstantTable(constantTable), verb_table(verbTable),
    SeenLabels(SeenLabels), inside_loop(inside_loop), inside_block(inside_block) {
    return;
}

void code_visitor::visit(compound_statement& s) {
    for(auto& stmt : *s.code) {
        stmt->accept(*this);
    }
}

void code_visitor::visit(empty_statement&) {
    return;
}

void code_visitor::visit(expression_statement& s) {
    expression_visitor v(symbol_table, ConstantTable, verb_table);
    s.expr->accept(v);
}

void code_visitor::visit(break_statement&) {
    if(!inside_loop) {
        LOG_ERROR("break used outside loop");
    }
}

void code_visitor::visit(return_statement&) {
    return;
}

void code_visitor::visit(call_statement& s) {
    // Convert label name to lowercase for processing
    std::transform(s.label.begin(), s.label.end(), s.label.begin(), tolower);

    if(SeenLabels.find(s.label) == SeenLabels.end()) {
        LOG_ERROR(format("label '%s' called but not defined") % s.label);
    }
}

void code_visitor::visit(if_statement& s) {
    expression_visitor cond(symbol_table, ConstantTable, verb_table);
    s.condition->accept(cond);

    if(cond.expression_type != vm::type::boolean) {
        diagnostic_context dc(*s.condition);
        LOG_WARNING("implicit cast to bool used in condition");
    }

    code_visitor v(symbol_table, ConstantTable, verb_table, SeenLabels, inside_loop, true);
    s.code->accept(v);
}

void code_visitor::visit(if_else_statement& s) {
    expression_visitor cond(symbol_table, ConstantTable, verb_table);
    s.condition->accept(cond);

    if(cond.expression_type != vm::type::boolean) {
        diagnostic_context dc(*s.condition);
        LOG_WARNING("implicit cast to bool used in condition");
    }

    code_visitor first(symbol_table, ConstantTable, verb_table, SeenLabels, inside_loop, true);
    s.code->accept(first);

    code_visitor second(symbol_table, ConstantTable, verb_table, SeenLabels, inside_loop, true);
    s.else_code->accept(second);
}

void code_visitor::visit(while_statement& s) {
    expression_visitor cond(symbol_table, ConstantTable, verb_table);
    s.condition->accept(cond);

    if(cond.expression_type != vm::type::boolean) {
        diagnostic_context dc(*s.condition);
        LOG_WARNING("implicit cast to bool used in condition");
    }

    code_visitor v(symbol_table, ConstantTable, verb_table, SeenLabels, true, true);
    s.code->accept(v);
}

void code_visitor::visit(do_statement& s) {
    expression_visitor cond(symbol_table, ConstantTable, verb_table);
    s.condition->accept(cond);

    if(cond.expression_type != vm::type::boolean) {
        diagnostic_context dc(*s.condition);
        LOG_WARNING("implicit cast to bool used in condition");
    }

    code_visitor v(symbol_table, ConstantTable, verb_table, SeenLabels, true, true);
    s.code->accept(v);
}

void code_visitor::visit(for_statement& s) {
    expression_visitor init(symbol_table, ConstantTable, verb_table);
    s.initializer->accept(init);

    expression_visitor cond(symbol_table, ConstantTable, verb_table);
    s.condition->accept(cond);

    if(cond.expression_type != vm::type::boolean) {
        diagnostic_context dc(*s.condition);
        LOG_WARNING("implicit cast to bool used in condition");
    }

    expression_visitor inc(symbol_table, ConstantTable, verb_table);
    s.incrementer->accept(inc);

    code_visitor v(symbol_table, ConstantTable, verb_table, SeenLabels, true, true);
    s.code->accept(v);
}

void code_visitor::visit(labeled_statement& s) {
    if(inside_block) {
        LOG_WARNING(format("label '%s' is defined inside block") % s.label);
    }

    s.code->accept(*this);
}
