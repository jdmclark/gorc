#include "expression_visitor.h"

using namespace Gorc::Cog::AST;
using Gorc::Cog::Stages::ConstantFolding::ExpressionVisitor;

ExpressionVisitor::ExpressionVisitor(AST::Factory& factory, Symbols::SymbolTable& symbolTable,
	const std::unordered_map<std::string, VM::Value>& constantTable, Diagnostics::Report& report)
	: AST::Visitor("Stage3::ExpressionVisitor", report), Factory(factory), SymbolTable(symbolTable), ConstantTable(constantTable) {
	return;
}

void ExpressionVisitor::VisitStringLiteralExpression(StringLiteralExpression& e) {
	visitedExpression = &e;
	isConstant = true;
	constantValue = VM::Value(SymbolTable.AddString(e.Value));
}

void ExpressionVisitor::VisitIntegerLiteralExpression(IntegerLiteralExpression& e) {
	visitedExpression = &e;
	isConstant = true;
	constantValue = VM::Value(e.Value);
}

void ExpressionVisitor::VisitFloatLiteralExpression(FloatLiteralExpression& e) {
	visitedExpression = &e;
	isConstant = true;
	constantValue = VM::Value(e.Value);
}

void ExpressionVisitor::VisitVectorLiteralExpression(VectorLiteralExpression& e) {
	visitedExpression = &e;
	isConstant = true;
	constantValue = VM::Value(Math::Vector<float>(e.X, e.Y, e.Z));
}

void ExpressionVisitor::VisitIdentifierExpression(IdentifierExpression& e) {
	visitedExpression = &e;

	auto it = ConstantTable.find(e.Identifier);
	if(it != ConstantTable.end()) {
		isConstant = true;
		constantValue = it->second;
	}
	else {
		isConstant = false;
	}
}

void ExpressionVisitor::VisitSubscriptExpression(SubscriptExpression& e) {
	visitedExpression = &e;
	isConstant = false;

	ExpressionVisitor v(Factory, SymbolTable, ConstantTable, Report);
	e.Index->Accept(v);
	e.Index = v.GetSubstitution();
}

void ExpressionVisitor::VisitMethodCallExpression(MethodCallExpression& e) {
	visitedExpression = &e;
	isConstant = false;

	for(size_t i = 0; i < e.Arguments->size(); ++i) {
		ExpressionVisitor v(Factory, SymbolTable, ConstantTable, Report);
		(*e.Arguments)[i]->Accept(v);
		(*e.Arguments)[i] = v.GetSubstitution();
	}
}

void ExpressionVisitor::VisitUnaryExpression(UnaryExpression& e) {
	visitedExpression = &e;

	ExpressionVisitor v(Factory, SymbolTable, ConstantTable, Report);
	e.Base->Accept(v);

	if(v.IsConstant()) {
		isConstant = true;

		switch(e.Operator) {
		case UnaryOperator::Plus:
			constantValue = +v.GetConstantValue();
			break;

		case UnaryOperator::Minus:
			constantValue = -v.GetConstantValue();
			break;

		case UnaryOperator::Not:
			constantValue = !v.GetConstantValue();
			break;
		}
	}
	else {
		isConstant = false;
	}
}

void ExpressionVisitor::VisitInfixExpression(InfixExpression& e) {
	visitedExpression = &e;

	ExpressionVisitor v(Factory, SymbolTable, ConstantTable, Report);
	e.Left->Accept(v);

	ExpressionVisitor w(Factory, SymbolTable, ConstantTable, Report);
	e.Right->Accept(w);

	if(v.IsConstant() && w.IsConstant()) {
		isConstant = true;

		switch(e.Operator) {
		case InfixOperator::Addition:
			constantValue = v.GetConstantValue() + w.GetConstantValue();
			break;

		case InfixOperator::Subtraction:
			constantValue = v.GetConstantValue() - w.GetConstantValue();
			break;

		case InfixOperator::Multiplication:
			constantValue = v.GetConstantValue() * w.GetConstantValue();
			break;

		case InfixOperator::Division:
			constantValue = v.GetConstantValue() / w.GetConstantValue();
			break;

		case InfixOperator::Modulo:
			constantValue = v.GetConstantValue() % w.GetConstantValue();
			break;

		case InfixOperator::Greater:
			constantValue = v.GetConstantValue() > w.GetConstantValue();
			break;

		case InfixOperator::GreaterEqual:
			constantValue = v.GetConstantValue() >= w.GetConstantValue();
			break;

		case InfixOperator::Less:
			constantValue = v.GetConstantValue() < w.GetConstantValue();
			break;

		case InfixOperator::LessEqual:
			constantValue = v.GetConstantValue() <= w.GetConstantValue();
			break;

		case InfixOperator::Equal:
			constantValue = v.GetConstantValue() == w.GetConstantValue();
			break;

		case InfixOperator::NotEqual:
			constantValue = v.GetConstantValue() != w.GetConstantValue();
			break;

		case InfixOperator::And:
			constantValue = v.GetConstantValue() & w.GetConstantValue();
			break;

		case InfixOperator::Or:
			constantValue = v.GetConstantValue() | w.GetConstantValue();
			break;

		case InfixOperator::Xor:
			constantValue = v.GetConstantValue() ^ w.GetConstantValue();
			break;

		case InfixOperator::LogicalAnd:
			constantValue = v.GetConstantValue() && w.GetConstantValue();
			break;

		case InfixOperator::LogicalOr:
			constantValue = v.GetConstantValue() || w.GetConstantValue();
			break;
		}
	}
	else {
		isConstant = false;

		e.Left = v.GetSubstitution();
		e.Right = w.GetSubstitution();
	}
}

void ExpressionVisitor::VisitAssignmentExpression(AssignmentExpression& e) {
	visitedExpression = &e;
	isConstant = false;

	ExpressionVisitor w(Factory, SymbolTable, ConstantTable, Report);
	e.Target->Accept(w);

	ExpressionVisitor v(Factory, SymbolTable, ConstantTable, Report);
	e.Value->Accept(v);
	e.Value = v.GetSubstitution();
}

void ExpressionVisitor::VisitCommaExpression(CommaExpression& e) {
	visitedExpression = &e;
	isConstant = false;

	ExpressionVisitor v(Factory, SymbolTable, ConstantTable, Report);
	
	e.Left->Accept(v);
	e.Left = v.GetSubstitution();

	e.Right->Accept(v);
	e.Right = v.GetSubstitution();
}

void ExpressionVisitor::VisitForOptionalExpression(ForOptionalExpression& e) {
	return;
}

void ExpressionVisitor::VisitForExpression(ForExpression& e) {
	ExpressionVisitor v(Factory, SymbolTable, ConstantTable, Report);

	e.Condition->Accept(v);
	e.Condition = v.GetSubstitution();
}
