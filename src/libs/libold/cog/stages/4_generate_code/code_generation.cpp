#include "libold/cog/symbols/table.hpp"
#include "libold/cog/verbs/table.hpp"
#include "libold/cog/ir/printer.hpp"
#include "libold/cog/ast/node.hpp"
#include "code_visitor.hpp"

#include "libold/cog/stages/stages.hpp"

void gorc::cog::stages::generate_code::generate_code(ast::translation_unit* ast,
                                                     ir::printer& printer,
                                                     std::unordered_map<std::string, vm::value> const &ct)
{
    printer.comment("==============================");
    printer.comment("COG compiler diagnostic output");
    printer.comment("==============================");

    // Generate 'default' action for unimplemented messages.
    printer.ret();

    int nextLabelNumber = 1;
    code_visitor v(nextLabelNumber, printer, ct);

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
