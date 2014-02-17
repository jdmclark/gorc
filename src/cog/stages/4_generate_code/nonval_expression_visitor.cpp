#include "nonval_expression_visitor.h"
#include "lval_expression_visitor.h"
#include "expression_visitor.h"

using namespace gorc::cog::ast;
using gorc::cog::stages::generate_code::non_valued_expression_visitor;

non_valued_expression_visitor::non_valued_expression_visitor(ir::printer& printer, diagnostics::report& report)
    : ast::visitor("Stage5::NonvaluedExpressionVisitor", report), Printer(printer) {
    return;
}

void non_valued_expression_visitor::visit_identifier_expression(identifier_expression& e) {
    return;
}

void non_valued_expression_visitor::visit_subscript_expression(subscript_expression& e) {
    e.index->accept(*this);
    return;
}

void non_valued_expression_visitor::visit_method_call_expression(method_call_expression& e) {
    expression_visitor v(Printer, report);
    for(auto& arg : *e.arguments) {
        arg->accept(v);
    }

    Printer.call(e.base);
}

void non_valued_expression_visitor::visit_unary_expression(unary_expression& e) {
    e.base->accept(*this);
}

void non_valued_expression_visitor::visit_infix_expression(infix_expression& e) {
    e.left->accept(*this);
    e.right->accept(*this);
}

void non_valued_expression_visitor::visit_assignment_expression(assignment_expression& e) {
    expression_visitor v(Printer, report);
    e.value->accept(v);

    lvalue_expression_visitor lv(Printer, report);
    e.target->accept(lv);
}

void non_valued_expression_visitor::visit_comma_expression(comma_expression& e) {
    e.left->accept(*this);
    e.right->accept(*this);
}

void non_valued_expression_visitor::visit_for_optional_expression(for_optional_expression& e) {
    return;
}

void non_valued_expression_visitor::visit_for_expression(for_expression& e) {
    e.condition->accept(*this);
}

void non_valued_expression_visitor::visit_constant_value_expression(constant_value_expression& e) {
    return;
}
