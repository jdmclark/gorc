#include "program/program.hpp"
#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "io/native_file.hpp"

namespace gorc {

    class boc_shell_program_visitor {
    public:
        int visit(translation_unit &)
        {
            return EXIT_FAILURE;
        }
    };

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
            diagnostic_context dc(script_filename.c_str());
            auto nf = make_native_read_only_file(script_filename);
            ast_factory ast;
            shell_tokenizer tok(*nf);
            translation_unit *tu = parse_shell_script(ast, tok);

            if(parse_only) {
                return EXIT_SUCCESS;
            }

            boc_shell_program_visitor visitor;
            return ast_visit(visitor, tu);
        }
    };

}

MAKE_MAIN(gorc::boc_shell_program)
