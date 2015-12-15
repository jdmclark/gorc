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
    statement* parse_compound_statement(ast_factory &ast, shell_la_tokenizer &tok);
    expression* parse_expression(ast_factory &ast, shell_la_tokenizer &tok);

    /* Words */

    argument* parse_argument(ast_factory &ast, shell_la_tokenizer &tok)
    {
        ast_list_node<word*> *words =
            ast.make<ast_list_node<word*>>(tok.get_location());

        while(true) {
            if(tok.get_type() == shell_token_type::punc_begin_expr) {
                auto start_loc = tok.get_location();
                tok.advance();

                expression *value = parse_expression(ast, tok);

                if(tok.get_type() != shell_token_type::punc_end_expr) {
                    diagnostic_context dc(tok.get_location());
                    LOG_FATAL(format("expected ')', found '%s'") % tok.get_value());
                }

                auto end_loc = tok.get_location();
                tok.advance();

                words->elements.push_back(
                        ast.make_var<word, expression_word>(location_union(start_loc, end_loc),
                                                            value));
                words->location = location_union(words->location,
                                                 end_loc);
            }
            else if(tok.get_type() == shell_token_type::word) {
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

    command* parse_pipe_command(ast_factory &ast, shell_la_tokenizer &tok)
    {
        ast_list_node<subcommand*> *subcommands =
            ast.make<ast_list_node<subcommand*>>(tok.get_location());

        maybe<pipe_io_redirection*> stdin_source;
        maybe<pipe_io_redirection*> stdout_target;
        maybe<pipe_io_redirection*> stderr_target;

        while(true) {
            subcommand *subcmd = parse_subcommand(ast, tok);
            subcommands->elements.push_back(subcmd);
            subcommands->location = location_union(subcommands->location,
                                                   subcmd->location);

            if(tok.get_type() == shell_token_type::punc_pipe) {
                // Subcommand is followed by another
                tok.advance();
            }
            else {
                break;
            }
        }

        // Parse redirections
        while(true) {
            if(tok.get_type() == shell_token_type::punc_stdin_redirect) {
                if(stdin_source.has_value()) {
                    diagnostic_context dc(tok.get_location());
                    LOG_FATAL("stdin already redirected");
                }

                auto start_loc = tok.get_location();
                tok.advance();

                argument *dest = parse_argument(ast, tok);

                stdin_source = ast.make<pipe_io_redirection>(
                        location_union(start_loc, dest->location),
                        false,
                        dest);
            }
            else if(tok.get_type() == shell_token_type::punc_stdout_redirect) {
                if(stdout_target.has_value()) {
                    diagnostic_context dc(tok.get_location());
                    LOG_FATAL("stdout already redirected");
                }

                auto start_loc = tok.get_location();
                tok.advance();

                argument *dest = parse_argument(ast, tok);

                stdout_target = ast.make<pipe_io_redirection>(
                        location_union(start_loc, dest->location),
                        false,
                        dest);
            }
            else if(tok.get_type() == shell_token_type::punc_stdout_append) {
                if(stdout_target.has_value()) {
                    diagnostic_context dc(tok.get_location());
                    LOG_FATAL("stdout already redirected");
                }

                auto start_loc = tok.get_location();
                tok.advance();

                argument *dest = parse_argument(ast, tok);

                stdout_target = ast.make<pipe_io_redirection>(
                        location_union(start_loc, dest->location),
                        true,
                        dest);
            }
            else if(tok.get_type() == shell_token_type::punc_stderr_redirect) {
                if(stderr_target.has_value()) {
                    diagnostic_context dc(tok.get_location());
                    LOG_FATAL("stderr already redirected");
                }

                auto start_loc = tok.get_location();
                tok.advance();

                argument *dest = parse_argument(ast, tok);

                stderr_target = ast.make<pipe_io_redirection>(
                        location_union(start_loc, dest->location),
                        false,
                        dest);
            }
            else if(tok.get_type() == shell_token_type::punc_stderr_append) {
                if(stderr_target.has_value()) {
                    diagnostic_context dc(tok.get_location());
                    LOG_FATAL("stderr already redirected");
                }

                auto start_loc = tok.get_location();
                tok.advance();

                argument *dest = parse_argument(ast, tok);

                stderr_target = ast.make<pipe_io_redirection>(
                        location_union(start_loc, dest->location),
                        true,
                        dest);
            }
            else {
                break;
            }
        }

        return ast.make_var<command, pipe_command>(subcommands->location,
                                                   subcommands,
                                                   stdin_source,
                                                   stdout_target,
                                                   stderr_target);
    }

    command* parse_and_infix_command(ast_factory &ast, shell_la_tokenizer &tok)
    {
        command *rv = parse_pipe_command(ast, tok);
        while(tok.get_type() == shell_token_type::punc_logical_and) {
            tok.advance();
            command *right = parse_pipe_command(ast, tok);
            rv = ast.make_var<command, infix_command>(
                    location_union(ast_visit(variant_location_visitor(), *rv),
                                   ast_visit(variant_location_visitor(), *right)),
                    command_infix_operator::logical_and,
                    rv,
                    right);
        }

        return rv;
    }

    command* parse_or_infix_command(ast_factory &ast, shell_la_tokenizer &tok)
    {
        command *rv = parse_and_infix_command(ast, tok);
        while(tok.get_type() == shell_token_type::punc_logical_or) {
            tok.advance();
            command *right = parse_and_infix_command(ast, tok);
            rv = ast.make_var<command, infix_command>(
                    location_union(ast_visit(variant_location_visitor(), *rv),
                                   ast_visit(variant_location_visitor(), *right)),
                    command_infix_operator::logical_or,
                    rv,
                    right);
        }

        return rv;
    }

    /* Expressions */

    expression* parse_expression(ast_factory &ast, shell_la_tokenizer &tok);

    expression* parse_argument_expression(ast_factory &ast, shell_la_tokenizer &tok)
    {
        argument *arg = parse_argument(ast, tok);
        return ast.make_var<expression, argument_expression>(arg->location, arg);
    }

    expression* parse_call_expression(ast_factory &ast, shell_la_tokenizer &tok)
    {
        simple_word *name = ast.make<simple_word>(tok.get_location(), tok.get_value());
        tok.advance();

        // lparen - by look-ahead
        ast_list_node<expression*> *args = ast.make<ast_list_node<expression*>>(tok.get_location());
        tok.advance();

        if(tok.get_type() != shell_token_type::punc_end_expr) {
            while(true) {
                args->elements.push_back(parse_expression(ast, tok));
                args->location = location_union(args->location,
                                                ast_visit(variant_location_visitor(),
                                                          *args->elements.back()));

                if(tok.get_type() == shell_token_type::punc_comma) {
                    tok.advance();
                }
                else if(tok.get_type() == shell_token_type::punc_end_expr) {
                    break;
                }
                else {
                    diagnostic_context dc(tok.get_location());
                    LOG_FATAL(format("expected ',' or ')', found '%s'") % tok.get_value());
                }
            }
        }

        // rparen
        auto end_loc = tok.get_location();
        tok.advance();

        return ast.make_var<expression, call_expression>(location_union(name->location, end_loc),
                                                         name,
                                                         args);
    }

    expression* parse_primary_expression(ast_factory &ast, shell_la_tokenizer &tok)
    {
        if(tok.get_type() == shell_token_type::kw_nil) {
            expression *rv = ast.make_var<expression, nil_expression>(tok.get_location());
            tok.advance();
            return rv;
        }
        else if(tok.get_type() == shell_token_type::word &&
                tok.get_token(1).type == shell_token_type::punc_begin_expr) {
            return parse_call_expression(ast, tok);
        }
        else if(tok.get_type() == shell_token_type::punc_begin_expr) {
            tok.advance();

            expression *rv = parse_expression(ast, tok);

            if(tok.get_type() != shell_token_type::punc_end_expr) {
                diagnostic_context dc(tok.get_location());
                LOG_FATAL(format("expected ')', found '%s'") % tok.get_value());
            }

            tok.advance();

            return rv;
        }
        else {
            return parse_argument_expression(ast, tok);
        }
    }

    expression* parse_unary_expression(ast_factory &ast, shell_la_tokenizer &tok)
    {
        maybe<unary_operator> op;

        switch(tok.get_type()) {
        case shell_token_type::punc_not:
            op = unary_operator::logical_not;
            break;

        case shell_token_type::kw_car:
            op = unary_operator::car;
            break;

        case shell_token_type::kw_cdr:
            op = unary_operator::cdr;
            break;

        case shell_token_type::kw_null:
            op = unary_operator::null;
            break;

        case shell_token_type::kw_atom:
            op = unary_operator::atom;
            break;

        default:
            // Deliberately not handled
            break;
        }

        if(op.has_value()) {
            auto start_loc = tok.get_location();
            tok.advance();

            expression *rv = parse_unary_expression(ast, tok);

            return ast.make_var<expression, unary_expression>(
                    location_union(start_loc, ast_visit(variant_location_visitor(), *rv)),
                    rv,
                    op.get_value());
        }
        else {
            return parse_primary_expression(ast, tok);
        }
    }

    expression* parse_cons_expression(ast_factory &ast, shell_la_tokenizer &tok)
    {
        expression *rv = parse_unary_expression(ast, tok);
        while(tok.get_type() == shell_token_type::punc_cons) {
            tok.advance();
            expression *right = parse_unary_expression(ast, tok);
            rv = ast.make_var<expression, infix_expression>(
                    location_union(ast_visit(variant_location_visitor(), *rv),
                                   ast_visit(variant_location_visitor(), *right)),
                    rv,
                    right,
                    infix_operator::cons);
        }

        return rv;
    }

    expression* parse_equality_expression(ast_factory &ast, shell_la_tokenizer &tok)
    {
        expression *rv = parse_cons_expression(ast, tok);
        while(tok.get_type() == shell_token_type::punc_equal ||
              tok.get_type() == shell_token_type::punc_notequal) {
            infix_operator op = infix_operator::equal;
            switch(tok.get_type()) {
            case shell_token_type::punc_equal:
                op = infix_operator::equal;
                break;

            case shell_token_type::punc_notequal:
                op = infix_operator::not_equal;
                break;

// LCOV_EXCL_START
            default:
                LOG_FATAL("unhandled infix operator");
            }
// LCOV_EXCL_STOP

            tok.advance();
            expression *right = parse_cons_expression(ast, tok);
            rv = ast.make_var<expression, infix_expression>(
                    location_union(ast_visit(variant_location_visitor(), *rv),
                                   ast_visit(variant_location_visitor(), *right)),
                    rv,
                    right,
                    op);
        }

        return rv;
    }

    expression* parse_expression(ast_factory &ast, shell_la_tokenizer &tok)
    {
        return parse_equality_expression(ast, tok);
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

    statement* parse_function_declaration_statement(ast_factory &ast, shell_la_tokenizer &tok)
    {
        // Current token is 'function'
        auto start_loc = tok.get_location();
        tok.advance();

        simple_word *name = ast.make<simple_word>(tok.get_location(), tok.get_value());
        tok.advance();

        if(tok.get_type() != shell_token_type::punc_begin_expr) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected '(', found '%s'") % tok.get_value());
        }

        ast_list_node<simple_word*> *args =
            ast.make<ast_list_node<simple_word*>>(tok.get_location());

        tok.advance();

        while(true) {
            if(tok.get_type() == shell_token_type::word) {
                args->elements.push_back(
                        ast.make<simple_word>(tok.get_location(), tok.get_value()));
                args->location = location_union(start_loc, tok.get_location());
                tok.advance();
            }
            else if(tok.get_type() == shell_token_type::punc_comma) {
                tok.advance();
            }
            else {
                break;
            }
        }

        if(tok.get_type() != shell_token_type::punc_end_expr) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ')', found '%s'") % tok.get_value());
        }

        tok.advance();

        statement *code = parse_compound_statement(ast, tok);

        return ast.make_var<statement, func_declaration_statement>(
                location_union(start_loc, ast_visit(variant_location_visitor(), *code)),
                name,
                args,
                code);
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

        maybe<ast_list_node<argument*>*> value;

        if(tok.get_type() == shell_token_type::punc_assign) {
            // Variable has a default value
            tok.advance();

            value = parse_argument_list(ast, tok);
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

        ast_list_node<argument*> *value = parse_argument_list(ast, tok);

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
        command *cmd = parse_or_infix_command(ast, tok);

        if(tok.get_type() != shell_token_type::punc_end_command) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected argument list, found '%s'") % tok.get_value());
        }

        auto end_loc = tok.get_location();
        tok.advance();

        return ast.make_var<statement, command_statement>(
                location_union(ast_visit(variant_location_visitor(), *cmd), end_loc),
                cmd);
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

    statement* parse_if_statement(ast_factory &ast, shell_la_tokenizer &tok)
    {
        // On 'if'
        auto start_loc = tok.get_location();
        tok.advance();

        if(tok.get_type() != shell_token_type::punc_begin_expr) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected '(', found '%s'") % tok.get_value());
        }

        tok.advance();

        expression *cond = parse_expression(ast, tok);

        if(tok.get_type() != shell_token_type::punc_end_expr) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ')', found '%s'") % tok.get_value());
        }

        tok.advance();

        statement* body = parse_compound_statement(ast, tok);

        if(tok.get_type() == shell_token_type::kw_else) {
            tok.advance();

            statement* else_body = nullptr;
            if(tok.get_type() == shell_token_type::kw_if) {
                else_body = parse_if_statement(ast, tok);
            }
            else if(tok.get_type() == shell_token_type::punc_begin_block) {
                else_body = parse_compound_statement(ast, tok);
            }
            else {
                diagnostic_context dc(tok.get_location());
                LOG_FATAL(format("expected block or 'if' after 'else', found '%s'") %
                          tok.get_value());
            }

            return ast.make_var<statement, if_else_statement>(
                    location_union(start_loc, ast_visit(variant_location_visitor(), *else_body)),
                    cond,
                    body,
                    else_body);
        }
        else {
            return ast.make_var<statement, if_statement>(
                    location_union(start_loc, ast_visit(variant_location_visitor(), *body)),
                    cond,
                    body);
        }
    }

    statement* parse_for_statement(ast_factory &ast, shell_la_tokenizer &tok)
    {
        // On 'for'
        auto start_loc = tok.get_location();
        tok.advance();

        simple_word *word = ast.make<simple_word>(tok.get_location(), tok.get_value());
        tok.advance();

        if(tok.get_type() == shell_token_type::punc_whitespace) {
            // Skip whitespace
            tok.advance();
        }

        if(tok.get_type() != shell_token_type::kw_in) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected 'in', found '%s'") % tok.get_value());
        }

        tok.advance();

        expression *list = parse_expression(ast, tok);
        statement* body = parse_compound_statement(ast, tok);

        return ast.make_var<statement, for_statement>(
                location_union(start_loc, ast_visit(variant_location_visitor(), *body)),
                word,
                list,
                body);
    }

    statement* parse_return_statement(ast_factory &ast, shell_la_tokenizer &tok)
    {
        auto start_loc = tok.get_location();

        // Already on 'return'
        tok.advance();

        maybe<expression*> value;

        if(tok.get_type() != shell_token_type::punc_end_command) {
            value = parse_expression(ast, tok);
        }

        if(tok.get_type() != shell_token_type::punc_end_command) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ';', found '%s'") % tok.get_value());
        }

        auto end_loc = tok.get_location();
        tok.advance();

        return ast.make_var<statement, return_statement>(location_union(start_loc, end_loc),
                                                         value);
    }

    statement* parse_call_statement(ast_factory &ast, shell_la_tokenizer &tok)
    {
        // Currently on call token
        auto start_loc = tok.get_location();
        tok.advance();

        expression *value = parse_expression(ast, tok);

        if(tok.get_type() != shell_token_type::punc_end_command) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ';', found '%s'") % tok.get_value());
        }

        auto end_loc = tok.get_location();
        tok.advance();

        return ast.make_var<statement, call_statement>(location_union(start_loc, end_loc),
                                                       value);
    }

    statement* parse_statement(ast_factory &ast, shell_la_tokenizer &tok)
    {
        if(tok.get_type() == shell_token_type::kw_include) {
            return parse_include_statement(ast, tok);
        }
        else if(tok.get_type() == shell_token_type::kw_var) {
            return parse_var_declaration_statement(ast, tok);
        }
        else if(tok.get_type() == shell_token_type::kw_function) {
            return parse_function_declaration_statement(ast, tok);
        }
        else if(tok.get_type() == shell_token_type::kw_return) {
            return parse_return_statement(ast, tok);
        }
        else if(tok.get_type() == shell_token_type::kw_call) {
            return parse_call_statement(ast, tok);
        }
        else if(tok.get_type() == shell_token_type::kw_if) {
            return parse_if_statement(ast, tok);
        }
        else if(tok.get_type() == shell_token_type::kw_for) {
            return parse_for_statement(ast, tok);
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
