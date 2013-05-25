#include "lval_visitor.h"
#include "expression_visitor.h"
#include "framework/diagnostics/helper.h"
#include <algorithm>

using namespace Gorc::Cog::AST;
using Gorc::Cog::Stages::SemanticAnalysis::LValueVisitor;

LValueVisitor::LValueVisitor(Symbols::SymbolTable& symbolTable,
	const std::unordered_map<std::string, VM::Value>& constantTable,
	Verbs::VerbTable& verbTable, Diagnostics::Report& report)
	: AST::Visitor("Stage2::LValueVisitor", report), SymbolTable(symbolTable),
	ConstantTable(constantTable), VerbTable(verbTable) {
	return;
}

void LValueVisitor::DefaultAction(const std::string& action, Node& n) {
	Diagnostics::Helper::AssignToRValue(Report, VisitorName, n.Location);
}

void LValueVisitor::VisitIdentifierExpression(IdentifierExpression& e) {
	// Convert identifier to lowercase for processing.
	std::transform(e.Identifier.begin(), e.Identifier.end(), e.Identifier.begin(), tolower);

	if(!SymbolTable.IsSymbolDefined(e.Identifier)) {
		Diagnostics::Helper::UndefinedSymbol(Report, VisitorName, e.Identifier, e.Location);
		SymbolTable.AddSymbol(e.Identifier);
	}
}

void LValueVisitor::VisitSubscriptExpression(SubscriptExpression& e) {
	// Convert base identifier to lowercase for processing.
	std::transform(e.Base.begin(), e.Base.end(), e.Base.begin(), tolower);

	if(!SymbolTable.IsSymbolDefined(e.Base)) {
		Diagnostics::Helper::UndefinedSymbol(Report, VisitorName, e.Base, e.Location);
		SymbolTable.AddSymbol(e.Base);
	}

	ExpressionVisitor subscript(SymbolTable, ConstantTable, VerbTable, Report);
	e.Index->Accept(subscript);

	if(subscript.ExpressionType == VM::Type::Void) {
		Diagnostics::Helper::IllegalVoidResult(Report, VisitorName, e.Index->Location);
	}
}
