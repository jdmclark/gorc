#include "program/program.hpp"
#include "ast.hpp"
#include "parser.hpp"
#include "io/native_file.hpp"
#include "system/process.hpp"
#include "system/pipe.hpp"
#include "system/env.hpp"
#include <algorithm>
#include <unordered_map>

namespace gorc {

    std::unordered_map<std::string, std::string> variable_map;

    class boc_shell_word_visitor {
    public:
        std::string visit(simple_word &w) {
            return w.value;
        }

        std::string visit(variable_name &var) {
            auto it = variable_map.find(var.name);
            if(it != variable_map.end()) {
                return it->second;
            }

            auto value = get_environment_variable(var.name);
            if(value.has_value()) {
                return value.get_value();
            }
            else {
                LOG_FATAL(format("variable '%s' is undefined") % var.name);
            }
        }
    };

    class boc_shell_argument_visitor {
    public:
        std::string visit(argument &arg) {
            boc_shell_word_visitor bswv;
            std::string assembled_arg;
            for(auto &part : arg.words->elements) {
                assembled_arg += ast_visit(bswv, *part);
            }

            return assembled_arg;
        }
    };

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

            boc_shell_argument_visitor bsav;
            while(sub_it != cmd.subcommands->elements.end() &&
                  stdin_it != stdin_pipes.end() &&
                  stdout_it != stdout_pipes.end()) {
                auto &sub_args = (*sub_it)->arguments->elements;

                // First token is the program to execute
                std::string prog = ast_visit(bsav, sub_args.front());
                std::vector<std::string> args;
                for(auto it = sub_args.begin() + 1;
                    it != sub_args.end();
                    ++it) {
                    args.push_back(ast_visit(bsav, *it));
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

        int visit(assignment_statement &s)
        {
            boc_shell_argument_visitor bsav;
            variable_map.emplace(s.var->name, ast_visit(bsav, s.value));

            return EXIT_SUCCESS;
        }

        int visit(export_statement &s)
        {
            auto it = variable_map.find(s.var->name);
            if(it == variable_map.end()) {
                LOG_FATAL(format("variable '%s' is undefined") % s.var->name);
            }

            set_environment_variable(s.var->name, it->second);
            return EXIT_SUCCESS;
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

        int visit(compound_statement &stmt)
        {
            return ast_visit(*this, stmt.code);
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
            ast_factory ast;
            translation_unit *tu = parse_shell_script(script_filename, ast);

            if(parse_only) {
                return EXIT_SUCCESS;
            }

            boc_shell_program_visitor visitor;
            return ast_visit(visitor, tu);
        }
    };

}

MAKE_MAIN(gorc::boc_shell_program)
