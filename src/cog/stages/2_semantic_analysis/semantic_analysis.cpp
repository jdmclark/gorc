#include "cog/ast/node.h"
#include "cog/symbols/table.h"
#include "cog/verbs/table.h"
#include "framework/diagnostics/report.h"

#include "symbol_visitor.h"
#include "code_visitor.h"
#include "label_visitor.h"

#include "cog/stages/stages.h"

void Gorc::Cog::Stages::SemanticAnalysis::SemanticAnalysis(AST::TranslationUnit* ast, Symbols::SymbolTable& symbolTable,
	const std::unordered_map<std::string, VM::Value>& constantTable, Verbs::VerbTable& verbTable, Diagnostics::Report& report) {
	std::unordered_set<std::string> seenLabels;
	LabelVisitor labelVisitor(seenLabels, report);

	for(auto& stmt : *ast->Code) {
		stmt->Accept(labelVisitor);
	}

	SymbolVisitor symbolVisitor(symbolTable, seenLabels, report);

	for(auto& symbol : *ast->Symbols) {
		symbol->Accept(symbolVisitor);
	}

	CodeVisitor codeVisitor(symbolTable, constantTable, verbTable, seenLabels, report);

	for(auto& stmt : *ast->Code) {
		stmt->Accept(codeVisitor);
	}
}
