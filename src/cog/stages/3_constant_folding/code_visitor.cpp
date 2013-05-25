#include "code_visitor.h"
#include "expression_visitor.h"

using namespace Gorc::Cog::AST;
using Gorc::Cog::Stages::ConstantFolding::CodeVisitor;

CodeVisitor::CodeVisitor(AST::Factory& factory, Symbols::SymbolTable& symbolTable,
	const std::unordered_map<std::string, VM::Value>& constantTable,
	Diagnostics::Report& report)
	: AST::Visitor("Stage3::CodeVisitor", report), Factory(factory), SymbolTable(symbolTable),
	ConstantTable(constantTable) {
	return;
}

void CodeVisitor::VisitCompoundStatement(CompoundStatement& s) {
	for(auto& stmt : *s.Code) {
		stmt->Accept(*this);
	}
}

void CodeVisitor::VisitEmptyStatement(EmptyStatement& s) {
	return;
}

void CodeVisitor::VisitExpressionStatement(ExpressionStatement& s) {
	ExpressionVisitor v(Factory, SymbolTable, ConstantTable, Report);
	s.Expression->Accept(v);
	s.Expression = v.GetSubstitution();
}

void CodeVisitor::VisitBreakStatement(BreakStatement& s) {
	return;
}

void CodeVisitor::VisitReturnStatement(ReturnStatement& s) {
	return;
}

void CodeVisitor::VisitCallStatement(CallStatement& s) {
	return;
}

void CodeVisitor::VisitIfStatement(IfStatement& s) {
	ExpressionVisitor v(Factory, SymbolTable, ConstantTable, Report);
	s.Condition->Accept(v);
	s.Condition = v.GetSubstitution();

	s.Code->Accept(*this);
}

void CodeVisitor::VisitIfElseStatement(IfElseStatement& s) {
	ExpressionVisitor v(Factory, SymbolTable, ConstantTable, Report);
	s.Condition->Accept(v);
	s.Condition = v.GetSubstitution();

	s.Code->Accept(*this);
	s.ElseCode->Accept(*this);
}

void CodeVisitor::VisitWhileStatement(WhileStatement& s) {
	ExpressionVisitor v(Factory, SymbolTable, ConstantTable, Report);
	s.Condition->Accept(v);
	s.Condition = v.GetSubstitution();

	s.Code->Accept(*this);
}

void CodeVisitor::VisitDoStatement(DoStatement& s) {
	ExpressionVisitor v(Factory, SymbolTable, ConstantTable, Report);
	s.Condition->Accept(v);
	s.Condition = v.GetSubstitution();

	s.Code->Accept(*this);
}

void CodeVisitor::VisitForStatement(ForStatement& s) {
	ExpressionVisitor v(Factory, SymbolTable, ConstantTable, Report);

	s.Condition->Accept(v);
	s.Incrementer->Accept(v);
	s.Initializer->Accept(v);

	s.Code->Accept(*this);
}

void CodeVisitor::VisitLabeledStatement(LabeledStatement& s) {
	s.Code->Accept(*this);
}
