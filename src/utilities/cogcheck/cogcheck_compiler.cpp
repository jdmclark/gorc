#include "cogcheck_compiler.hpp"
#include "print_ast.hpp"
#include "disassembler.hpp"

gorc::cogcheck_compiler::cogcheck_compiler(cog::verb_table &verbs,
                                           cog::constant_table &constants,
                                           bool dump_ast,
                                           bool parse_only,
                                           bool disassemble)
    : compiler(verbs, constants)
    , dump_ast(dump_ast)
    , parse_only(parse_only)
    , disassemble(disassemble)
{
    return;
}

bool gorc::cogcheck_compiler::handle_parsed_ast(cog::ast::translation_unit &tu)
{
    if(parse_only && dump_ast) {
        cog::script junk;
        print_ast(tu, junk);
    }

    return !parse_only;
}

bool gorc::cogcheck_compiler::handle_analyzed_ast(cog::ast::translation_unit &tu,
                                                  cog::script &script)
{
    if(dump_ast) {
        print_ast(tu, script);
    }

    return disassemble;
}

bool gorc::cogcheck_compiler::handle_generated_code(cog::script &script)
{
    if(disassemble) {
        disassemble_code(script, verbs);
    }

    return true;
}
