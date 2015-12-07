#include "program/program.hpp"
#include "ast.hpp"
#include "parser.hpp"
#include "io/native_file.hpp"
#include "system/process.hpp"
#include "system/pipe.hpp"
#include "system/env.hpp"
#include "utility/range.hpp"
#include "stack.hpp"
#include <algorithm>
#include <unordered_map>
#include <vector>

namespace gorc {

    class boc_shell_assign_lvalue_visitor {
    public:
        std::string const &value;

        boc_shell_assign_lvalue_visitor(std::string const &value)
            : value(value)
        {
            return;
        }

        void visit(variable_name &var) const
        {
            set_variable_value(var.name, value);
        }

        void visit(environment_variable_name &var) const
        {
            set_environment_variable(var.name, value);
        }
    };

    class boc_shell_word_visitor {
    public:
        std::string visit(simple_word &w)
        {
            return w.value;
        }

        std::string visit(variable_name &var)
        {
            return get_variable_value(var.name);
        }

        std::string visit(environment_variable_name &var)
        {
            auto value = get_environment_variable(var.name);
            if(value.has_value()) {
                return value.get_value();
            }
            else {
                LOG_FATAL(format("environment variable '%s' is undefined") % var.name);
            }
        }
    };

    class boc_shell_argument_visitor {
    public:
        std::string visit(argument &arg) const
        {
            boc_shell_word_visitor bswv;
            std::string assembled_arg;
            for(auto &part : arg.words->elements) {
                assembled_arg += ast_visit(bswv, *part);
            }

            return assembled_arg;
        }
    };

    class boc_shell_expression_visitor {
    public:
        std::string visit(argument_expression &e) const
        {
            return ast_visit(boc_shell_argument_visitor(), e.value);
        }

        std::string visit(unary_expression &e) const
        {
            auto sub_value = ast_visit(boc_shell_expression_visitor(), *e.value);

            switch(e.op) {
            case unary_operator::logical_not:
                if(sub_value == "true") {
                    return "false";
                }
                else {
                    return "true";
                }

// LCOV_EXCL_START
            }

            // Not coverable
            LOG_FATAL("unhandled unary operator");

// LCOV_EXCL_STOP
        }

        std::string visit(infix_expression &e) const
        {
            auto left_value = ast_visit(boc_shell_expression_visitor(), *e.left);
            auto right_value = ast_visit(boc_shell_expression_visitor(), *e.right);

            switch(e.op) {
            case infix_operator::equal:
                if(left_value == right_value) {
                    return "true";
                }
                else {
                    return "false";
                }

            case infix_operator::not_equal:
                if(left_value != right_value) {
                    return "true";
                }
                else {
                    return "false";
                }
// LCOV_EXCL_START
            }

            // Not coverable
            LOG_FATAL("unhandled infix operator");

// LCOV_EXCL_STOP
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
            std::string value = ast_visit(bsav, s.value);

            ast_visit(boc_shell_assign_lvalue_visitor(value), *s.var);

            return EXIT_SUCCESS;
        }

        int visit(var_declaration_statement &s)
        {
            std::string value;
            if(s.value.has_value()) {
                boc_shell_argument_visitor bsav;
                value = ast_visit(bsav, s.value.get_value());
            }

            create_variable(s.var->name, value);

            return EXIT_SUCCESS;
        }

        int visit(if_statement &s)
        {
            auto cond = ast_visit(boc_shell_expression_visitor(), *s.condition);
            if(cond == "true") {
                ast_visit(*this, *s.code);
            }

            return EXIT_SUCCESS;
        }

        int visit(if_else_statement &s)
        {
            auto cond = ast_visit(boc_shell_expression_visitor(), *s.condition);
            if(cond == "true") {
                ast_visit(*this, *s.code);
            }
            else {
                ast_visit(*this, *s.elsecode);
            }

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
            scoped_stack_frame sf;
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
