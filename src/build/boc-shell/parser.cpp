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

    /* Statements */

    statement* parse_statement(ast_factory &, shell_la_tokenizer &tok);

    statement* parse_command_statement(ast_factory &ast, shell_la_tokenizer &tok)
    {
        auto start_loc = tok.get_location();

        ast_list_node<subcommand*> *subcommands =
            ast.make<ast_list_node<subcommand*>>(start_loc);

        ast_list_node<word*> *arguments =
            ast.make<ast_list_node<word*>>(start_loc);

        while(true) {
            if(tok.get_type() == shell_token_type::punc_pipe) {
                // Finish current subcommand, start new one
                if(arguments->elements.empty()) {
                    diagnostic_context dc(tok.get_location());
                    LOG_FATAL(format("expected command word, found '%s'") % tok.get_value());
                }

                subcommands->elements.push_back(
                        ast.make<subcommand>(arguments->location, arguments));
                subcommands->location = location_union(subcommands->location,
                                                       tok.get_location());
                arguments = ast.make<ast_list_node<word*>>(start_loc);
                tok.advance();
            }
            else if(tok.get_type() == shell_token_type::punc_end_command) {
                // Finish current subcommand and return
                if(arguments->elements.empty()) {
                    diagnostic_context dc(tok.get_location());
                    LOG_FATAL(format("expected command word, found '%s'") % tok.get_value());
                }

                subcommands->elements.push_back(
                        ast.make<subcommand>(arguments->location, arguments));
                subcommands->location = location_union(subcommands->location,
                                                       tok.get_location());
                break;
            }
            else if(tok.get_type() == shell_token_type::word) {
                arguments->elements.push_back(ast.make<word>(tok.get_location(),
                                                             tok.get_value()));
                arguments->location = location_union(arguments->location,
                                                     tok.get_location());
                tok.advance();
            }
            else {
                diagnostic_context dc(tok.get_location());
                LOG_FATAL(format("expected argument list, found '%s'") % tok.get_value());
            }
        }

        auto end_loc = tok.get_location();
        tok.advance();

        command *cmd = ast.make_var<command, pipe_command>(location_union(start_loc, end_loc),
                                                           subcommands);

        return ast.make_var<statement, command_statement>(location_union(start_loc, end_loc), cmd);
    }

    statement* parse_statement(ast_factory &ast, shell_la_tokenizer &tok)
    {
        // TODO: Other statement types
        return parse_command_statement(ast, tok);
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
