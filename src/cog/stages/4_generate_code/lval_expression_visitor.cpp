#include "lval_expression_visitor.h"
#include "expression_visitor.h"

using namespace Gorc::Cog::AST;
using Gorc::Cog::Stages::GenerateCode::LValueExpressionVisitor;

LValueExpressionVisitor::LValueExpressionVisitor(IR::Printer& printer, Diagnostics::Report& report)
	: AST::Visitor("Stage5::LValueExpressionVisitor", report), Printer(printer) {
	return;
}

void LValueExpressionVisitor::VisitIdentifierExpression(IdentifierExpression& e) {
	Printer.Store(e.Identifier);
}

void LValueExpressionVisitor::VisitSubscriptExpression(SubscriptExpression& e) {
	ExpressionVisitor v(Printer, Report);
	e.Index->Accept(v);
	Printer.StoreI(e.Base);
}
