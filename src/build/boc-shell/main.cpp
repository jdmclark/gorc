#include "program/program.hpp"
#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "io/native_file.hpp"
#include "system/process.hpp"
#include <algorithm>

namespace gorc {

    class boc_shell_program_visitor {
    public:
        int visit(command_statement &cmd)
        {
            if(cmd.arguments->elements.empty()) {
                return EXIT_SUCCESS;
            }

            // First token is the program to execute
            std::string prog = cmd.arguments->elements.front()->value;
            std::vector<std::string> args;
            for(auto it = cmd.arguments->elements.begin() + 1;
                it != cmd.arguments->elements.end();
                ++it) {
                args.push_back((*it)->value);
            }

            process subprocess(prog,
                               args,
                               /* cin */ nothing,
                               /* out */ nothing,
                               /* err */ nothing);

            return subprocess.join();
        }

        int visit(ast_list_node<statement*> &stmt_seq)
        {
            for(auto &stmt : stmt_seq.elements) {
                int result = ast_visit(*this, *stmt);
                if(result != 0) {
                    // Abort executing a statement sequence after the first error
                    LOG_ERROR(format("command failed with code %d") % result);
                    return EXIT_FAILURE;
                }
            }

            return EXIT_SUCCESS;
        }

        int visit(translation_unit &tu)
        {
            return ast_visit(*this, tu.code);
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
