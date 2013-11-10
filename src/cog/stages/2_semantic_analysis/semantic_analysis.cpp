#include "cog/ast/node.h"
#include "cog/symbols/table.h"
#include "cog/verbs/table.h"
#include "framework/diagnostics/report.h"

#include "symbol_visitor.h"
#include "code_visitor.h"
#include "label_visitor.h"

#include "cog/stages/stages.h"

void gorc::cog::stages::semantic_analysis::semantic_analysis(ast::translation_unit* ast, symbols::symbol_table& symbolTable,
	const std::unordered_map<std::string, vm::value>& constantTable, verbs::verb_table& verbTable, diagnostics::report& report) {
	std::unordered_set<std::string> seenLabels;
	label_visitor labelVisitor(seenLabels, report);

	for(auto& stmt : *ast->code) {
		stmt->accept(labelVisitor);
	}

	symbol_visitor symbolVisitor(symbolTable, seenLabels, report);

	for(auto& symbol : *ast->symbols) {
		symbol->accept(symbolVisitor);
	}

	code_visitor codeVisitor(symbolTable, constantTable, verbTable, seenLabels, report);

	for(auto& stmt : *ast->code) {
		stmt->accept(codeVisitor);
	}
}
