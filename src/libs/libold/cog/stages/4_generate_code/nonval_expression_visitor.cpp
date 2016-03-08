#include "nonval_expression_visitor.hpp"
#include "lval_expression_visitor.hpp"
#include "expression_visitor.hpp"

using namespace gorc::cog::ast;
using gorc::cog::stages::generate_code::non_valued_expression_visitor;

non_valued_expression_visitor::non_valued_expression_visitor(ir::printer& printer,
                                                             std::unordered_map<std::string, vm::value> const &ct)
    : Printer(printer)
    , constant_table(ct) {
    return;
}

void non_valued_expression_visitor::visit(string_literal_expression&) {
    return;
}

void non_valued_expression_visitor::visit(integer_literal_expression&) {
    return;
}

void non_valued_expression_visitor::visit(float_literal_expression&) {
    return;
}

void non_valued_expression_visitor::visit(vector_literal_expression&) {
    return;
}

void non_valued_expression_visitor::visit(identifier_expression&) {
    return;
}

void non_valued_expression_visitor::visit(subscript_expression& e) {
    e.index->accept(*this);
    return;
}

void non_valued_expression_visitor::visit(method_call_expression& e) {
    expression_visitor v(Printer, constant_table);
    for(auto& arg : *e.arguments) {
        arg->accept(v);
    }

    Printer.call(e.base);
}

void non_valued_expression_visitor::visit(unary_expression& e) {
    e.base->accept(*this);
}

void non_valued_expression_visitor::visit(infix_expression& e) {
    e.left->accept(*this);
    e.right->accept(*this);
}

void non_valued_expression_visitor::visit(assignment_expression& e) {
    expression_visitor v(Printer, constant_table);
    e.value->accept(v);

    lvalue_expression_visitor lv(Printer, constant_table);
    e.target->accept(lv);
}

void non_valued_expression_visitor::visit(comma_expression& e) {
    e.left->accept(*this);
    e.right->accept(*this);
}

void non_valued_expression_visitor::visit(for_empty_expression&) {
    return;
}

void non_valued_expression_visitor::visit(for_expression& e) {
    e.condition->accept(*this);
}
