#include "parser.hpp"
#include "utility/string_search.hpp"
#include "ast/variant_location_visitor.hpp"
#include "utility/string_view.hpp"
#include "text/lookahead_tokenizer.hpp"
#include "io/native_file.hpp"
#include "ast/factory.hpp"
#include "lexer.hpp"
#include <stack>
#include <sstream>
#include <boost/filesystem.hpp>

using namespace gorc;

namespace {

    // Helpers

    std::stack<char const *> current_filename_stack;
    std::stack<path> current_path_stack;

    using shell_la_tokenizer = token_lookahead<shell_tokenizer>;

    diagnostic_context_location location_union(diagnostic_context_location const &first,
                                               diagnostic_context_location const &last)
    {
        return diagnostic_context_location(first.filename,
                                           first.first_line,
                                           first.first_col,
                                           last.last_line,
                                           last.last_col);
    }

    ast_list_node<statement*>* parse_shell_script_file(path const &filename, ast_factory &ast);

    /* Words */

    argument* parse_argument(ast_factory &ast, shell_la_tokenizer &tok)
    {
        ast_list_node<word*> *words =
            ast.make<ast_list_node<word*>>(tok.get_location());

        while(true) {
            if(tok.get_type() == shell_token_type::word) {
                words->elements.push_back(
                        ast.make_var<word, simple_word>(tok.get_location(),
                                                        tok.get_value()));
                words->location = location_union(words->location,
                                                 tok.get_location());
                tok.advance();
            }
            else if(tok.get_type() == shell_token_type::variable_name) {
                words->elements.push_back(
                        ast.make_var<word, variable_name>(tok.get_location(),
                                                          tok.get_value()));
                words->location = location_union(words->location,
                                                 tok.get_location());
                tok.advance();
            }
            else if(tok.get_type() == shell_token_type::environment_variable_name) {
                words->elements.push_back(
                        ast.make_var<word, environment_variable_name>(tok.get_location(),
                                                                      tok.get_value()));
                words->location = location_union(words->location,
                                                 tok.get_location());
                tok.advance();
            }
            else {
                if(words->elements.empty()) {
                    diagnostic_context dc(tok.get_location());
                    LOG_FATAL(format("expected command word, found '%s'") % tok.get_value());
                }
                break;
            }
        }

        return ast.make<argument>(words->location, words);
    }

    ast_list_node<argument*>* parse_argument_list(ast_factory &ast, shell_la_tokenizer &tok)
    {
        ast_list_node<argument*> *arguments =
            ast.make<ast_list_node<argument*>>(tok.get_location());

        while(true) {
            arguments->elements.push_back(parse_argument(ast, tok));
            arguments->location = location_union(arguments->location,
                                                 ast_visit(variant_location_visitor(),
                                                           arguments->elements.back()));

            if(tok.get_type() == shell_token_type::punc_whitespace) {
                // Ignore whitespace
                tok.advance();
            }

            if(tok.get_type() != shell_token_type::word &&
               tok.get_type() != shell_token_type::variable_name &&
               tok.get_type() != shell_token_type::environment_variable_name) {
                return arguments;
            }
        }
    }

    subcommand* parse_subcommand(ast_factory &ast, shell_la_tokenizer &tok)
    {
        ast_list_node<argument*> *arguments =
            parse_argument_list(ast, tok);

        return ast.make<subcommand>(arguments->location, arguments);
    }

    /* Statements */

    lvalue* parse_lvalue(ast_factory &ast, shell_la_tokenizer &tok)
    {
        if(tok.get_type() == shell_token_type::variable_name) {
            lvalue *rv = ast.make_var<lvalue, variable_name>(tok.get_location(),
                                                             tok.get_value());
            tok.advance();
            return rv;
        }
        else if(tok.get_type() == shell_token_type::environment_variable_name) {
            lvalue *rv = ast.make_var<lvalue, environment_variable_name>(tok.get_location(),
                                                                         tok.get_value());
            tok.advance();
            return rv;
        }
        else {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected lvalue, found '%s'") % tok.get_value());
        }
    }

    statement* parse_var_declaration_statement(ast_factory &ast, shell_la_tokenizer &tok)
    {
        // Current token is "var"
        auto start_loc = tok.get_location();
        tok.advance();

        if(tok.get_type() != shell_token_type::variable_name) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected variable name, found '%s'") % tok.get_value());
        }

        variable_name *vn = ast.make<variable_name>(tok.get_location(), tok.get_value());
        tok.advance();

        maybe<argument*> value;

        if(tok.get_type() == shell_token_type::punc_assign) {
            // Variable has a default value
            tok.advance();

            value = parse_argument(ast, tok);
        }

        if(tok.get_type() != shell_token_type::punc_end_command) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ';', found '%s'") % tok.get_value());
        }

        auto end_loc = tok.get_location();
        tok.advance();

        return ast.make_var<statement, var_declaration_statement>(
                location_union(start_loc, end_loc),
                vn,
                value);
    }

    statement* parse_assignment_statement(ast_factory &ast, shell_la_tokenizer &tok)
    {
        lvalue *vn = parse_lvalue(ast, tok);

        // Equals - already checked via lookahead
        tok.advance();

        argument *value = parse_argument(ast, tok);

        if(tok.get_type() != shell_token_type::punc_end_command) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ';', found '%s'") % tok.get_value());
        }

        statement *rv = ast.make_var<statement, assignment_statement>(
                location_union(ast_visit(variant_location_visitor(), *vn), tok.get_location()),
                vn,
                value);
        tok.advance();
        return rv;
    }

    statement* parse_include_statement(ast_factory &ast, shell_la_tokenizer &tok)
    {
        // On 'include'
        tok.advance();

        if(tok.get_type() != shell_token_type::word) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected constant filename, found '%s'") % tok.get_value());
        }

        std::string fn = tok.get_value();
        tok.advance();

        if(tok.get_type() != shell_token_type::punc_end_command) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ';', found '%s'") % tok.get_value());
        }

        tok.advance();

        // Process include
        ast_list_node<statement*> *contents = parse_shell_script_file(fn, ast);

        return ast.make_var<statement, compound_statement>(
                contents->location,
                contents);
    }

    statement* parse_command_statement(ast_factory &ast, shell_la_tokenizer &tok)
    {
        ast_list_node<subcommand*> *subcommands =
            ast.make<ast_list_node<subcommand*>>(tok.get_location());

        while(true) {
            subcommand *subcmd = parse_subcommand(ast, tok);
            subcommands->elements.push_back(subcmd);
            subcommands->location = location_union(subcommands->location,
                                                   subcmd->location);

            if(tok.get_type() == shell_token_type::punc_pipe) {
                // Subcommand is followed by another
                tok.advance();
            }
            else if(tok.get_type() == shell_token_type::punc_end_command) {
                break;
            }
            else {
                diagnostic_context dc(tok.get_location());
                LOG_FATAL(format("expected argument list, found '%s'") % tok.get_value());
            }
        }

        auto end_loc = tok.get_location();
        tok.advance();

        command *cmd = ast.make_var<command, pipe_command>(location_union(subcommands->location,
                                                                          end_loc),
                                                           subcommands);

        return ast.make_var<statement, command_statement>(location_union(subcommands->location,
                                                                         end_loc), cmd);
    }

    statement* parse_statement(ast_factory &ast, shell_la_tokenizer &tok);

    ast_list_node<statement*>* parse_comp_stmt_seq(ast_factory &ast, shell_la_tokenizer &tok)
    {
        auto start_loc = tok.get_location();

        ast_list_node<statement*> *code =
            ast.make<ast_list_node<statement*>>(start_loc);

        while(true) {
            if(tok.get_type() == shell_token_type::punc_end_block) {
                break;
            }

            code->elements.push_back(parse_statement(ast, tok));
            code->location = location_union(code->location,
                                            ast_visit(variant_location_visitor(),
                                                      *code->elements.back()));
        }

        code->location.filename = current_filename_stack.top();

        return code;
    }

    statement* parse_compound_statement(ast_factory &ast, shell_la_tokenizer &tok)
    {
        // On open brace
        auto start_loc = tok.get_location();

        if(tok.get_type() != shell_token_type::punc_begin_block) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected '{', found '%s'") % tok.get_value());
        }

        tok.advance();

        ast_list_node<statement*> *code = parse_comp_stmt_seq(ast, tok);

        auto end_loc = tok.get_location();

        tok.advance();

        return ast.make_var<statement, compound_statement>(
                location_union(start_loc, end_loc),
                code);
    }

    statement* parse_statement(ast_factory &ast, shell_la_tokenizer &tok)
    {
        if(tok.get_value() == "include") {
            return parse_include_statement(ast, tok);
        }
        else if(tok.get_value() == "var") {
            return parse_var_declaration_statement(ast, tok);
        }
        else if(tok.get_type() == shell_token_type::punc_begin_block) {
            return parse_compound_statement(ast, tok);
        }
        else if(tok.get_token(1).type == shell_token_type::punc_assign) {
            return parse_assignment_statement(ast, tok);
        }
        else {
            return parse_command_statement(ast, tok);
        }
    }

    ast_list_node<statement*>* parse_global_stmt_seq(ast_factory &ast, shell_la_tokenizer &tok)
    {
        auto start_loc = tok.get_location();

        ast_list_node<statement*> *code =
            ast.make<ast_list_node<statement*>>(start_loc);

        while(true) {
            if(tok.get_type() == shell_token_type::end_of_file) {
                break;
            }

            code->elements.push_back(parse_statement(ast, tok));
            code->location = location_union(code->location,
                                            ast_visit(variant_location_visitor(),
                                                      *code->elements.back()));
        }

        code->location.filename = current_filename_stack.top();

        return code;
    }

    ast_list_node<statement*>* parse_shell_script_file(path const &filename, ast_factory &ast)
    {
        class scoped_fn_details {
        public:
            scoped_fn_details(char const *fn, path const &p)
            {
                current_filename_stack.push(fn);
                current_path_stack.push(p);
            }

            ~scoped_fn_details()
            {
                current_filename_stack.pop();
                current_path_stack.pop();
            }
        };

        path adjusted_filename;
        if(current_path_stack.empty() || filename.is_absolute()) {
            adjusted_filename = filename;
        }
        else {
            adjusted_filename = current_path_stack.top().parent_path() / filename;
        }

        std::string *fn_node = ast.make<std::string>(adjusted_filename.native());
        scoped_fn_details sfd(fn_node->c_str(), adjusted_filename);

        diagnostic_context dc(fn_node->c_str());
        std::unique_ptr<input_stream> nf;
        try {
            nf = make_native_read_only_file(adjusted_filename);
        }
        catch(std::exception const &e) {
            LOG_FATAL(e.what());
        }

        shell_tokenizer tok(*nf);
        shell_la_tokenizer latok(tok, 2);

        return parse_global_stmt_seq(ast, latok);
    }
}

translation_unit* gorc::parse_shell_script(path const &filename, ast_factory &ast)
{
    ast_list_node<statement*> *code = parse_shell_script_file(filename, ast);
    return ast.make<translation_unit>(code->location, code);
}
