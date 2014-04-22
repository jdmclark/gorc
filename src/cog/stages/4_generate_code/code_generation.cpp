#include "cog/symbols/table.h"
#include "cog/verbs/table.h"
#include "cog/ir/printer.h"
#include "cog/ast/node.h"
#include "base/diagnostics/report.h"
#include "code_visitor.h"

#include "cog/stages/stages.h"

void gorc::cog::stages::generate_code::generate_code(ast::translation_unit* ast, ir::printer& printer, diagnostics::report& report) {
    printer.comment("==============================");
    printer.comment("COG compiler diagnostic output");
    printer.comment("==============================");

    // Generate 'default' action for unimplemented messages.
    printer.ret();

    int nextLabelNumber = 1;
    code_visitor v(nextLabelNumber, printer, report);

    for(auto& stmt : *ast->code) {
        stmt->accept(v);
    }

    // Generate guard return
    if(ast->code->size() > 0) {
        printer.ret();
    }

    printer.backpatch();

    return;
}
