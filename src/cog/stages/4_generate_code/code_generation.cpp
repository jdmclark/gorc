#include "cog/symbols/table.h"
#include "cog/verbs/table.h"
#include "cog/ir/printer.h"
#include "cog/ast/node.h"
#include "framework/diagnostics/report.h"
#include "code_visitor.h"

#include "cog/stages/stages.h"

void Gorc::Cog::Stages::GenerateCode::GenerateCode(AST::TranslationUnit* ast, IR::Printer& printer, Diagnostics::Report& report) {
	printer.Comment("==============================");
	printer.Comment("COG compiler diagnostic output");
	printer.Comment("==============================");

	// Generate 'default' action for unimplemented messages.
	printer.Ret();

	int nextLabelNumber = 1;
	CodeVisitor v(nextLabelNumber, printer, report);

	for(auto& stmt : *ast->Code) {
		stmt->Accept(v);
	}

	// Generate guard return
	if(ast->Code->size() > 0) {
		printer.Ret();
	}

	printer.Backpatch();

	return;
}
