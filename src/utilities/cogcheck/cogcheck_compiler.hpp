#pragma once

#include "cog/compiler/compiler.hpp"

namespace gorc {

    class cogcheck_compiler : public cog::compiler {
    private:
        bool dump_ast = false;
        bool parse_only = false;
        bool disassemble = false;

    public:
        cogcheck_compiler(cog::verb_table &verbs,
                          cog::constant_table &constants,
                          bool dump_ast,
                          bool parse_only,
                          bool disassemble);

        virtual bool handle_parsed_ast(cog::ast::translation_unit &) override;
        virtual bool handle_analyzed_ast(cog::ast::translation_unit &, cog::script &) override;
        virtual bool handle_generated_code(cog::script &) override;
    };

}
