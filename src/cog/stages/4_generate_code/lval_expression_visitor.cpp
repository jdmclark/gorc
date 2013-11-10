#include "lval_expression_visitor.h"
#include "expression_visitor.h"

using namespace gorc::cog::ast;
using gorc::cog::stages::generate_code::lvalue_expression_visitor;

lvalue_expression_visitor::lvalue_expression_visitor(ir::printer& printer, diagnostics::report& report)
	: ast::visitor("Stage5::LvalueExpressionVisitor", report), Printer(printer) {
	return;
}

void lvalue_expression_visitor::visit_identifier_expression(identifier_expression& e) {
	Printer.store(e.identifier);
}

void lvalue_expression_visitor::visit_subscript_expression(subscript_expression& e) {
	expression_visitor v(Printer, report);
	e.index->accept(v);
	Printer.storei(e.base);
}
