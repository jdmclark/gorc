#include "program/program.hpp"
#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "io/native_file.hpp"
#include "system/process.hpp"
#include "system/pipe.hpp"
#include <algorithm>

namespace gorc {

    class boc_shell_program_visitor {
    public:
        int visit(pipe_command &cmd)
        {
            // Open interprocess pipes
            size_t num_subcommands = cmd.subcommands->elements.size();

            std::vector<std::unique_ptr<pipe>> pipes;
            for(size_t i = 1; i < num_subcommands; ++i) {
                pipes.push_back(make_unique<pipe>());
            }

            std::vector<maybe<pipe*>> stdin_pipes;
            std::vector<maybe<pipe*>> stdout_pipes;

            // First process takes stdin from console
            stdin_pipes.push_back(nothing); // TODO: stdin redirection

            for(auto &pipe : pipes) {
                stdin_pipes.push_back(pipe.get());
                stdout_pipes.push_back(pipe.get());
            }

            // Last process sends stdout to console
            stdout_pipes.push_back(nothing); // TODO: stdout redirection

            auto sub_it = cmd.subcommands->elements.begin();
            auto stdin_it = stdin_pipes.begin();
            auto stdout_it = stdout_pipes.begin();

            std::vector<std::unique_ptr<process>> processes;

            while(sub_it != cmd.subcommands->elements.end() &&
                  stdin_it != stdin_pipes.end() &&
                  stdout_it != stdout_pipes.end()) {
                auto &sub_args = (*sub_it)->arguments->elements;

                // First token is the program to execute
                std::string prog = sub_args.front()->value;
                std::vector<std::string> args;
                for(auto it = sub_args.begin() + 1;
                    it != sub_args.end();
                    ++it) {
                    args.push_back((*it)->value);
                }

                processes.push_back(make_unique<process>(prog,
                                                         args,
                                                         *stdin_it,
                                                         *stdout_it,
                                                         /* err */ nothing));
                ++sub_it;
                ++stdin_it;
                ++stdout_it;
            }

            int last_exit_code = 0;
            for(auto &proc : processes) {
                last_exit_code = proc->join();
            }

            return last_exit_code;
        }

        int visit(command_statement &cmd)
        {
            return ast_visit(*this, *cmd.cmd);
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
