#include "expression_visitor.h"
#include "lval_expression_visitor.h"
#include "nonval_expression_visitor.h"

using namespace Gorc::Cog::AST;
using Gorc::Cog::Stages::GenerateCode::ExpressionVisitor;

ExpressionVisitor::ExpressionVisitor(IR::Printer& printer, Diagnostics::Report& report)
	: AST::Visitor("Stage5::ExpressionVisitor", report), Printer(printer) {
	return;
}

void ExpressionVisitor::VisitIdentifierExpression(IdentifierExpression& e) {
	Printer.Load(e.Identifier);
}

void ExpressionVisitor::VisitSubscriptExpression(SubscriptExpression& e) {
	e.Index->Accept(*this);
	Printer.LoadI(e.Base);
}

void ExpressionVisitor::VisitMethodCallExpression(MethodCallExpression& e) {
	for(auto& arg : *e.Arguments) {
		arg->Accept(*this);
	}

	Printer.CallV(e.Base);
}

void ExpressionVisitor::VisitUnaryExpression(UnaryExpression& e) {
	e.Base->Accept(*this);
	
	switch(e.Operator) {
	case UnaryOperator::Plus:
		break;

	case UnaryOperator::Minus:
		Printer.Neg();
		break;

	case UnaryOperator::Not:
		Printer.LNot();
		break;
	}
}

void ExpressionVisitor::VisitInfixExpression(InfixExpression& e) {
	e.Left->Accept(*this);
	e.Right->Accept(*this);

	switch(e.Operator) {
	case InfixOperator::Addition:
		Printer.Add();
		break;

	case InfixOperator::Subtraction:
		Printer.Sub();
		break;

	case InfixOperator::Multiplication:
		Printer.Mul();
		break;

	case InfixOperator::Division:
		Printer.Div();
		break;

	case InfixOperator::Modulo:
		Printer.Mod();
		break;

	case InfixOperator::Greater:
		Printer.CGt();
		break;

	case InfixOperator::GreaterEqual:
		Printer.CGeq();
		break;

	case InfixOperator::Less:
		Printer.CLt();
		break;

	case InfixOperator::LessEqual:
		Printer.CLeq();
		break;

	case InfixOperator::Equal:
		Printer.CEq();
		break;

	case InfixOperator::NotEqual:
		Printer.CNeq();
		break;

	case InfixOperator::And:
		Printer.And();
		break;

	case InfixOperator::Or:
		Printer.Or();
		break;

	case InfixOperator::Xor:
		Printer.Xor();
		break;

	case InfixOperator::LogicalAnd:
		Printer.LAnd();
		break;

	case InfixOperator::LogicalOr:
		Printer.LOr();
		break;
	}
}

void ExpressionVisitor::VisitAssignmentExpression(AssignmentExpression& e) {
	e.Value->Accept(*this);

	// Copy to preserve value on stack after assignment
	Printer.Copy();

	LValueExpressionVisitor v(Printer, Report);
	e.Target->Accept(v);
}

void ExpressionVisitor::VisitCommaExpression(CommaExpression& e) {
	NonValuedExpressionVisitor v(Printer, Report);
	e.Left->Accept(v);
	e.Right->Accept(*this);
}

void ExpressionVisitor::VisitForOptionalExpression(ForOptionalExpression& e) {
	// For optional expression always has value of 'true'
	Printer.Const(true);
}

void ExpressionVisitor::VisitForExpression(ForExpression& e) {
	e.Condition->Accept(*this);
}

void ExpressionVisitor::VisitConstantValueExpression(ConstantValueExpression& e) {
	Printer.Const(e.Value);
}
