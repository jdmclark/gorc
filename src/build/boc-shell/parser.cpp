#include "parser.hpp"
#include "utility/string_search.hpp"
#include "ast/variant_location_visitor.hpp"
#include "utility/string_view.hpp"
#include "text/lookahead_tokenizer.hpp"
#include <sstream>

using namespace gorc;

namespace {

    // Helpers

    using shell_la_tokenizer = token_lookahead<shell_tokenizer>;

    diagnostic_context_location location_union(diagnostic_context_location const &first,
                                               diagnostic_context_location const &last)
    {
        return diagnostic_context_location(nothing,
                                           first.first_line,
                                           first.first_col,
                                           last.last_line,
                                           last.last_col);
    }

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
               tok.get_type() != shell_token_type::variable_name) {
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

    statement* parse_assignment_statement(ast_factory &ast, shell_la_tokenizer &tok)
    {
        variable_name *vn = ast.make<variable_name>(tok.get_location(),
                                                    tok.get_value());
        tok.advance();

        // Equals - already checked via lookahead
        tok.advance();

        argument *value = parse_argument(ast, tok);

        if(tok.get_type() != shell_token_type::punc_end_command) {
            LOG_FATAL(format("expected ';', found '%s'") % tok.get_value());
        }

        statement *rv = ast.make_var<statement, assignment_statement>(
                location_union(vn->location, tok.get_location()),
                vn,
                value);
        tok.advance();
        return rv;
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

    statement* parse_statement(ast_factory &ast, shell_la_tokenizer &tok)
    {
        // TODO: Other statement types
        if(tok.get_token(1).type == shell_token_type::punc_assign) {
            return parse_assignment_statement(ast, tok);
        }
        else {
            return parse_command_statement(ast, tok);
        }
    }

    translation_unit* parse_translation_unit(ast_factory &ast, shell_la_tokenizer &tok)
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

        return ast.make<translation_unit>(location_union(start_loc,
                                                         tok.get_location()),
                                          code);
    }

}

translation_unit* gorc::parse_shell_script(ast_factory &factory, shell_tokenizer &tok)
{
    shell_la_tokenizer latok(tok, 2);
    return parse_translation_unit(factory, latok);
}
