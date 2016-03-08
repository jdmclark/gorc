#include "libold/cog/ast/node.hpp"
#include "libold/cog/symbols/table.hpp"
#include "libold/cog/verbs/table.hpp"

#include "symbol_visitor.hpp"
#include "code_visitor.hpp"
#include "label_visitor.hpp"

#include "libold/cog/stages/stages.hpp"

void gorc::cog::stages::semantic_analysis::semantic_analysis(ast::translation_unit* ast, symbols::symbol_table& symbolTable,
    const std::unordered_map<std::string, vm::value>& constantTable, verbs::verb_table& verbTable) {
    std::unordered_set<std::string> seenLabels;
    label_visitor labelVisitor(seenLabels);

    for(auto& stmt : *ast->code) {
        stmt->accept(labelVisitor);
    }

    symbol_visitor symbolVisitor(symbolTable, seenLabels);

    for(auto& symbol : *ast->symbols) {
        symbol->accept(symbolVisitor);
    }

    code_visitor codeVisitor(symbolTable, constantTable, verbTable, seenLabels);

    for(auto& stmt : *ast->code) {
        stmt->accept(codeVisitor);
    }
}
