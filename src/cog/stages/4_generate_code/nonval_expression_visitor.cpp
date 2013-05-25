#include "nonval_expression_visitor.h"
#include "lval_expression_visitor.h"
#include "expression_visitor.h"

using namespace Gorc::Cog::AST;
using Gorc::Cog::Stages::GenerateCode::NonValuedExpressionVisitor;

NonValuedExpressionVisitor::NonValuedExpressionVisitor(IR::Printer& printer, Diagnostics::Report& report)
	: AST::Visitor("Stage5::NonValuedExpressionVisitor", report), Printer(printer) {
	return;
}

void NonValuedExpressionVisitor::VisitIdentifierExpression(IdentifierExpression& e) {
	return;
}

void NonValuedExpressionVisitor::VisitSubscriptExpression(SubscriptExpression& e) {
	e.Index->Accept(*this);
	return;
}

void NonValuedExpressionVisitor::VisitMethodCallExpression(MethodCallExpression& e) {
	ExpressionVisitor v(Printer, Report);
	for(auto& arg : *e.Arguments) {
		arg->Accept(v);
	}

	Printer.Call(e.Base);
}

void NonValuedExpressionVisitor::VisitUnaryExpression(UnaryExpression& e) {
	e.Base->Accept(*this);
}

void NonValuedExpressionVisitor::VisitInfixExpression(InfixExpression& e) {
	e.Left->Accept(*this);
	e.Right->Accept(*this);
}

void NonValuedExpressionVisitor::VisitAssignmentExpression(AssignmentExpression& e) {
	ExpressionVisitor v(Printer, Report);
	e.Value->Accept(v);

	LValueExpressionVisitor lv(Printer, Report);
	e.Target->Accept(lv);
}

void NonValuedExpressionVisitor::VisitCommaExpression(CommaExpression& e) {
	e.Left->Accept(*this);
	e.Right->Accept(*this);
}

void NonValuedExpressionVisitor::VisitForOptionalExpression(ForOptionalExpression& e) {
	return;
}

void NonValuedExpressionVisitor::VisitForExpression(ForExpression& e) {
	e.Condition->Accept(*this);
}

void NonValuedExpressionVisitor::VisitConstantValueExpression(ConstantValueExpression& e) {
	return;
}
