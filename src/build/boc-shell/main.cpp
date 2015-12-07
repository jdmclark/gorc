#include "program/program.hpp"
#include "program_visitor.hpp"
#include "ast/factory.hpp"
#include "parser.hpp"

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

        virtual int main() override
        {
            // Parse script file
            ast_factory ast;
            translation_unit *tu = parse_shell_script(script_filename, ast);

            if(parse_only) {
                return EXIT_SUCCESS;
            }

            return ast_visit(program_visitor(), tu);
        }
    };

}

MAKE_MAIN(gorc::boc_shell_program)
