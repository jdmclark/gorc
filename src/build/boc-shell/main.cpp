#include "program/program.hpp"
#include "program_visitor.hpp"
#include "symbol_visitor.hpp"
#include "ast/factory.hpp"
#include "parser.hpp"
#include "symbols.hpp"
#include "builtins.hpp"

namespace gorc {

    class boc_shell_program : public program {
    public:
        std::string script_filename;
        bool parse_only = false;

        virtual void create_options(options &opts) override
        {
            opts.insert(make_value_option("script", script_filename));
            opts.insert(make_switch_option("parse-only", parse_only));
            opts.emplace_constraint<required_option>("script");
            return;
        }

        virtual int run() override
        {
            register_builtins();

            // Parse script file
            ast_factory ast;
            translation_unit *tu = parse_shell_script(script_filename, ast);

            if(parse_only) {
                return EXIT_SUCCESS;
            }

            ast_visit(symbol_visitor(true), tu);

            program_visitor pg;
            ast_visit(pg, tu);
            return EXIT_SUCCESS;
        }
    };

}

MAKE_MAIN(gorc::boc_shell_program)
