#include "cog/ast/factory.h"
#include "base/diagnostics/report.h"
#include "cog/symbols/table.h"
#include <string>
#include <unordered_map>

#include "code_visitor.h"

#include "cog/stages/stages.h"

void gorc::cog::stages::constant_folding::constant_folding(ast::factory& factory, ast::translation_unit* ast, symbols::symbol_table& symbolTable,
    const std::unordered_map<std::string, vm::value>& constantTable, diagnostics::report& report) {
    code_visitor v(factory, symbolTable, constantTable, report);

    for(auto& stmt : *ast->code) {
        stmt->accept(v);
    }
}
