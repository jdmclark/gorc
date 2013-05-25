#include "code_visitor.h"
#include "expression_visitor.h"
#include "framework/diagnostics/helper.h"

#include <algorithm>

using namespace Gorc::Cog::AST;
using Gorc::Cog::Stages::SemanticAnalysis::CodeVisitor;

CodeVisitor::CodeVisitor(Symbols::SymbolTable& symbolTable,
	const std::unordered_map<std::string, VM::Value>& constantTable,
	Verbs::VerbTable& verbTable, const std::unordered_set<std::string>& SeenLabels,
	Diagnostics::Report& report, bool inside_loop, bool inside_block)
	: AST::Visitor("Stage2::CodeVisitor", report), SymbolTable(symbolTable),
	ConstantTable(constantTable), VerbTable(verbTable),
	SeenLabels(SeenLabels), inside_loop(inside_loop), inside_block(inside_block) {
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
	ExpressionVisitor v(SymbolTable, ConstantTable, VerbTable, Report);
	s.Expression->Accept(v);
}

void CodeVisitor::VisitBreakStatement(BreakStatement& s) {
	if(!inside_loop) {
		Diagnostics::Helper::BreakOutsideLoop(Report, VisitorName, s.Location);
	}
}

void CodeVisitor::VisitReturnStatement(ReturnStatement& s) {
	return;
}

void CodeVisitor::VisitCallStatement(CallStatement& s) {
	// Convert label name to lowercase for processing
	std::transform(s.Label.begin(), s.Label.end(), s.Label.begin(), tolower);

	if(SeenLabels.find(s.Label) == SeenLabels.end()) {
		Diagnostics::Helper::CallToUnusedLabel(Report, VisitorName, s.Label, s.Location);
	}
}

void CodeVisitor::VisitIfStatement(IfStatement& s) {
	ExpressionVisitor cond(SymbolTable, ConstantTable, VerbTable, Report);
	s.Condition->Accept(cond);

	if(cond.ExpressionType != VM::Type::Boolean) {
		Diagnostics::Helper::ResultNotBoolean(Report, VisitorName, s.Condition->Location);
	}

	CodeVisitor v(SymbolTable, ConstantTable, VerbTable, SeenLabels, Report, inside_loop, true);
	s.Code->Accept(v);
}

void CodeVisitor::VisitIfElseStatement(IfElseStatement& s) {
	ExpressionVisitor cond(SymbolTable, ConstantTable, VerbTable, Report);
	s.Condition->Accept(cond);

	if(cond.ExpressionType != VM::Type::Boolean) {
		Diagnostics::Helper::ResultNotBoolean(Report, VisitorName, s.Condition->Location);
	}

	CodeVisitor first(SymbolTable, ConstantTable, VerbTable, SeenLabels, Report, inside_loop, true);
	s.Code->Accept(first);

	CodeVisitor second(SymbolTable, ConstantTable, VerbTable, SeenLabels, Report, inside_loop, true);
	s.ElseCode->Accept(second);
}

void CodeVisitor::VisitWhileStatement(WhileStatement& s) {
	ExpressionVisitor cond(SymbolTable, ConstantTable, VerbTable, Report);
	s.Condition->Accept(cond);

	if(cond.ExpressionType != VM::Type::Boolean) {
		Diagnostics::Helper::ResultNotBoolean(Report, VisitorName, s.Condition->Location);
	}

	CodeVisitor v(SymbolTable, ConstantTable, VerbTable, SeenLabels, Report, true, true);
	s.Code->Accept(v);
}

void CodeVisitor::VisitDoStatement(DoStatement& s) {
	ExpressionVisitor cond(SymbolTable, ConstantTable, VerbTable, Report);
	s.Condition->Accept(cond);

	if(cond.ExpressionType != VM::Type::Boolean) {
		Diagnostics::Helper::ResultNotBoolean(Report, VisitorName, s.Condition->Location);
	}

	CodeVisitor v(SymbolTable, ConstantTable, VerbTable, SeenLabels, Report, true, true);
	s.Code->Accept(v);
}

void CodeVisitor::VisitForStatement(ForStatement& s) {
	ExpressionVisitor init(SymbolTable, ConstantTable, VerbTable, Report);
	s.Initializer->Accept(init);

	ExpressionVisitor cond(SymbolTable, ConstantTable, VerbTable, Report);
	s.Condition->Accept(cond);

	if(cond.ExpressionType != VM::Type::Boolean) {
		Diagnostics::Helper::ResultNotBoolean(Report, VisitorName, s.Condition->Location);
	}

	ExpressionVisitor inc(SymbolTable, ConstantTable, VerbTable, Report);
	s.Incrementer->Accept(inc);

	CodeVisitor v(SymbolTable, ConstantTable, VerbTable, SeenLabels, Report, true, true);
	s.Code->Accept(v);
}

void CodeVisitor::VisitLabeledStatement(LabeledStatement& s) {
	if(inside_block) {
		Diagnostics::Helper::LabelInsideBlock(Report, VisitorName, s.Label, s.Location);
	}

	s.Code->Accept(*this);
}
