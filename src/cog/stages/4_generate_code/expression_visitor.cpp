#include "expression_visitor.h"
#include "lval_expression_visitor.h"
#include "nonval_expression_visitor.h"

using namespace gorc::cog::ast;
using gorc::cog::stages::generate_code::expression_visitor;

expression_visitor::expression_visitor(ir::printer& printer, diagnostics::report& report)
    : ast::visitor("Stage5::ExpressionVisitor", report), Printer(printer) {
    return;
}

void expression_visitor::visit_identifier_expression(identifier_expression& e) {
    Printer.load(e.identifier);
}

void expression_visitor::visit_subscript_expression(subscript_expression& e) {
    e.index->accept(*this);
    Printer.loadi(e.base);
}

void expression_visitor::visit_method_call_expression(method_call_expression& e) {
    for(auto& arg : *e.arguments) {
        arg->accept(*this);
    }

    Printer.callv(e.base);
}

void expression_visitor::visit_unary_expression(unary_expression& e) {
    e.base->accept(*this);
    
    switch(e.op) {
    case unary_operator::plus:
        break;

    case unary_operator::minus:
        Printer.neg();
        break;

    case unary_operator::logical_not:
        Printer.lnot();
        break;
    }
}

void expression_visitor::visit_infix_expression(infix_expression& e) {
    e.left->accept(*this);
    e.right->accept(*this);

    switch(e.op) {
    case infix_operator::addition:
        Printer.add();
        break;

    case infix_operator::subtraction:
        Printer.sub();
        break;

    case infix_operator::multiplication:
        Printer.mul();
        break;

    case infix_operator::division:
        Printer.div();
        break;

    case infix_operator::modulo:
        Printer.mod();
        break;

    case infix_operator::greater:
        Printer.cgt();
        break;

    case infix_operator::greater_equal:
        Printer.cgeq();
        break;

    case infix_operator::less:
        Printer.clt();
        break;

    case infix_operator::less_equal:
        Printer.cleq();
        break;

    case infix_operator::equal:
        Printer.ceq();
        break;

    case infix_operator::not_equal:
        Printer.cneq();
        break;

    case infix_operator::bitwise_and:
        Printer.band();
        break;

    case infix_operator::bitwise_or:
        Printer.bor();
        break;

    case infix_operator::bitwise_xor:
        Printer.bxor();
        break;

    case infix_operator::logical_and:
        Printer.land();
        break;

    case infix_operator::logical_or:
        Printer.lor();
        break;
    }
}

void expression_visitor::visit_assignment_expression(assignment_expression& e) {
    e.value->accept(*this);

    // Copy to preserve value on stack after assignment
    Printer.copy();

    lvalue_expression_visitor v(Printer, report);
    e.target->accept(v);
}

void expression_visitor::visit_comma_expression(comma_expression& e) {
    non_valued_expression_visitor v(Printer, report);
    e.left->accept(v);
    e.right->accept(*this);
}

void expression_visitor::visit_for_optional_expression(for_optional_expression& e) {
    // For optional expression always has value of 'true'
    Printer.constant(true);
}

void expression_visitor::visit_for_expression(for_expression& e) {
    e.condition->accept(*this);
}

void expression_visitor::visit_constant_value_expression(constant_value_expression& e) {
    Printer.constant(e.value);
}
