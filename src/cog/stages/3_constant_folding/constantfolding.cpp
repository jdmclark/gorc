#include "cog/ast/factory.h"
#include "framework/diagnostics/report.h"
#include "cog/symbols/table.h"
#include <string>
#include <unordered_map>

#include "code_visitor.h"

#include "cog/stages/stages.h"

void Gorc::Cog::Stages::ConstantFolding::ConstantFolding(AST::Factory& factory, AST::TranslationUnit* ast, Symbols::SymbolTable& symbolTable,
	const std::unordered_map<std::string, VM::Value>& constantTable, Diagnostics::Report& report) {
	CodeVisitor v(factory, symbolTable, constantTable, report);

	for(auto& stmt : *ast->Code) {
		stmt->Accept(v);
	}
}
