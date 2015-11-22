#include "parser.hpp"
#include "utility/string_search.hpp"
#include "cog/ast/variant_location_visitor.hpp"
#include "utility/string_view.hpp"
#include "text/lookahead_tokenizer.hpp"
#include <sstream>

using namespace gorc;
using namespace gorc::cog;

namespace {

    // Helpers

    using cog_la_tokenizer = token_lookahead<cog_tokenizer>;

    template <typename TokenizerT>
    void assert_punctuator(TokenizerT &tok, cog_token_type type)
    {
        if(tok.get_type() != type) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("unexpected '%s'") % tok.get_value());
        }
    }

    template <typename T, typename TokenizerT>
    T get_number(TokenizerT &t)
    {
        bool format_success = false;
        T result = T(0);
        int64_t int_scratch;
        double dbl_scratch;

        std::stringstream ss;
        ss << t.get_value();

        switch(t.get_type()) {
        case cog_token_type::integer:
            // Uncomplicated
            format_success = !(ss >> result).fail();
            break;

        case cog_token_type::hex_integer:
            // Hex ints must be read into an integer type
            format_success = !(ss >> std::hex >> int_scratch).fail();
            result = static_cast<T>(int_scratch);
            break;

        case cog_token_type::real:
            // Reals must be read into a floating point type
            format_success = !(ss >> dbl_scratch).fail();
            result = static_cast<T>(dbl_scratch);
            break;

        default:
            format_success = false;
            break;
        }

        if(!format_success) {
            diagnostic_context dc(t.get_location());
            LOG_FATAL(format("expected a numeric value, found '%s'") % t.get_value());
        }

        return result;
    }

    diagnostic_context_location location_union(diagnostic_context_location const &first,
                                               diagnostic_context_location const &last)
    {
        return diagnostic_context_location(nothing,
                                           first.first_line,
                                           first.first_col,
                                           last.last_line,
                                           last.last_col);
    }

    // Symbols:
    ast::symbol_field* parse_symbol_field(ast::factory &ast, cog_tokenizer &tok)
    {
        // Current token is the assignment operator
        // Advance in symbol field mode
        tok.set_symbol_field_state();
        tok.advance();

        if(tok.get_type() == cog_token_type::integer ||
           tok.get_type() == cog_token_type::hex_integer) {
            ast::symbol_field *rv = ast.make_var<ast::symbol_field, ast::integer_field>(
                    tok.get_location(),
                    get_number<int>(tok));
            tok.advance();
            return rv;
        }
        else if(tok.get_type() == cog_token_type::real) {
            ast::symbol_field *rv = ast.make_var<ast::symbol_field, ast::float_field>(
                    tok.get_location(),
                    get_number<float>(tok));
            tok.advance();
            return rv;
        }
        else {
            ast::symbol_field *rv = ast.make_var<ast::symbol_field, ast::string_fragment_field>(
                    tok.get_location(),
                    tok.get_value());
            tok.advance();
            return rv;
        }
    }

    ast::symbol_extension* parse_symbol_extension(ast::factory &ast, cog_tokenizer &tok)
    {
        // Already sitting on extname
        ast::identifier *extension_name = ast.make<ast::identifier>(tok.get_location(),
                                                                    tok.get_value());
        tok.advance();

        if(tok.get_type() == cog_token_type::punc_assign) {
            // Valued extension
            ast::symbol_field *value = parse_symbol_field(ast, tok);
            return ast.make_var<ast::symbol_extension, ast::valued_extension>(
                    location_union(extension_name->location,
                                   ast::visit(variant_location_visitor(), *value)),
                    extension_name,
                    value);
        }
        else {
            return ast.make_var<ast::symbol_extension, ast::bare_extension>(
                    extension_name->location,
                    extension_name);
        }
    }

    ast::symbol* parse_symbol(ast::factory &ast, cog_tokenizer &tok)
    {
        // Already sitting on typename
        ast::identifier *symbol_type = ast.make<ast::identifier>(tok.get_location(),
                                                                 tok.get_value());
        tok.advance();

        if(tok.get_type() != cog_token_type::identifier) {
            diagnostic_context dc(tok.get_location());
            LOG_ERROR(format("expected symbol name, found '%s'") % tok.get_value());
        }

        ast::identifier *symbol_name = ast.make<ast::identifier>(tok.get_location(),
                                                                 tok.get_value());
        tok.advance();

        maybe<ast::symbol_field*> default_value;
        if(tok.get_type() == cog_token_type::punc_assign) {
            default_value = parse_symbol_field(ast, tok);
        }

        ast::list_node<ast::symbol_extension*> *exts =
            ast.make<ast::list_node<ast::symbol_extension*>>(tok.get_location());
        while(true) {
            if(tok.get_type() == cog_token_type::end_of_line ||
               tok.get_type() == cog_token_type::end_of_file) {
                break;
            }
            else if(tok.get_type() == cog_token_type::punc_comma) {
                // Ignore commas
                tok.advance();
                continue;
            }
            else if(tok.get_type() == cog_token_type::identifier) {
                exts->elements.push_back(parse_symbol_extension(ast, tok));
                exts->location = location_union(exts->location,
                                                ast::visit(variant_location_visitor(),
                                                           *exts->elements.back()));
            }
            else {
                diagnostic_context dc(tok.get_location());
                LOG_FATAL(format("expected extension name, found '%s'") % tok.get_value());
            }
        }

        ast::symbol *rv = ast.make<ast::symbol>(location_union(symbol_type->location,
                                                               tok.get_location()),
                                                symbol_type,
                                                symbol_name,
                                                exts,
                                                default_value);
        tok.advance();

        return rv;
    }

    ast::symbols_section* parse_symbols_section(ast::factory &ast, cog_tokenizer &tok)
    {
        class scoped_return_newlines {
        public:
            cog_tokenizer &tok;

            scoped_return_newlines(cog_tokenizer &tok)
                : tok(tok)
            {
                tok.return_newlines(true);
            }

            ~scoped_return_newlines()
            {
                tok.return_newlines(false);
            }
        };

        if(!iequal(tok.get_value(), "symbols"_sv)) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected 'symbols', found '%s'") % tok.get_value());
        }

        auto start_loc = tok.get_location();

        // Parse symbol seq
        ast::list_node<ast::symbol*> *symbols =
            ast.make<ast::list_node<ast::symbol*>>(start_loc);

        {
            scoped_return_newlines return_newlines(tok);
            tok.advance();

            while(true) {
                if(tok.get_type() == cog_token_type::end_of_line) {
                    // Skip empty lines
                    tok.advance();
                    continue;
                }
                else if(tok.get_type() == cog_token_type::end_of_file) {
                    diagnostic_context dc(tok.get_location());
                    LOG_FATAL("unexpected end of file inside symbols section");
                }
                else if(iequal(tok.get_value(), "end"_sv)) {
                    break;
                }
                else if(tok.get_type() == cog_token_type::identifier) {
                    // Valid type. Parse symbol.
                    symbols->elements.push_back(parse_symbol(ast, tok));
                    symbols->location = location_union(symbols->location,
                                                       symbols->elements.back()->location);
                }
                else {
                    diagnostic_context dc(tok.get_location());
                    LOG_FATAL(format("expected symbol type, found '%s'") % tok.get_value());
                }
            }
        }

        auto end_loc = tok.get_location();
        tok.advance();

        return ast.make<ast::symbols_section>(location_union(start_loc, end_loc),
                                              symbols);
    }

    // Expressions:

#define LEFT_ASSOCIATIVE_INFIX_EXPR(self_name, super_name, is_indicator) \
    ast::expression* parse_##self_name(ast::factory &ast, cog_la_tokenizer &tok) \
    { \
        ast::expression *rv = parse_##super_name(ast, tok); \
        while(is_indicator(tok.get_type())) { \
            auto indicator = tok.get_type(); \
            tok.advance(); \
            ast::expression *right = parse_##super_name(ast, tok); \
            rv = ast.make_var<ast::expression, ast::infix_expression>( \
                    location_union(ast::visit(variant_location_visitor(), *rv), \
                                   ast::visit(variant_location_visitor(), *right)), \
                    rv, \
                    right, \
                    infix_operator_map(indicator)); \
        } \
        return rv; \
    }

#define LEFT_ASSOCIATIVE_INFIX_EXPR_SIMPLE(self_name, super_name, indicator) \
    bool self_name##_is_indicator(cog_token_type t) \
    { \
        return t == indicator; \
    } \
    LEFT_ASSOCIATIVE_INFIX_EXPR(self_name, super_name, self_name##_is_indicator)

    ast::unary_operator unary_operator_map(cog_token_type t)
    {
        switch(t) {
        case cog_token_type::punc_excl:
            return ast::unary_operator::logical_not;

        case cog_token_type::punc_plus:
            return ast::unary_operator::plus;

        case cog_token_type::punc_minus:
            return ast::unary_operator::minus;

        // LCOV_EXCL_START
        default:
            LOG_FATAL("unimplemented unary operator");
        }
        // LCOV_EXCL_STOP
    }

    ast::infix_operator infix_operator_map(cog_token_type t)
    {
        switch(t) {
        case cog_token_type::punc_equal:
            return ast::infix_operator::equal;

        case cog_token_type::punc_notequal:
            return ast::infix_operator::not_equal;

        case cog_token_type::punc_greater:
            return ast::infix_operator::greater;

        case cog_token_type::punc_greaterequal:
            return ast::infix_operator::greater_equal;

        case cog_token_type::punc_less:
            return ast::infix_operator::less;

        case cog_token_type::punc_lessequal:
            return ast::infix_operator::less_equal;

        case cog_token_type::punc_plus:
            return ast::infix_operator::addition;

        case cog_token_type::punc_minus:
            return ast::infix_operator::subtraction;

        case cog_token_type::punc_times:
            return ast::infix_operator::multiplication;

        case cog_token_type::punc_div:
            return ast::infix_operator::division;

        case cog_token_type::punc_mod:
            return ast::infix_operator::modulo;

        case cog_token_type::punc_and:
            return ast::infix_operator::bitwise_and;

        case cog_token_type::punc_or:
            return ast::infix_operator::bitwise_or;

        case cog_token_type::punc_xor:
            return ast::infix_operator::bitwise_xor;

        case cog_token_type::punc_logical_and:
            return ast::infix_operator::logical_and;

        case cog_token_type::punc_logical_or:
            return ast::infix_operator::logical_or;

        // LCOV_EXCL_START
        default:
            LOG_FATAL("unimplemented infix operator");
        }
        // LCOV_EXCL_STOP
    }

    ast::expression* parse_expression(ast::factory &, cog_la_tokenizer &);
    ast::expression* parse_assignment_expression(ast::factory &, cog_la_tokenizer &);

    ast::expression* parse_literal_expression(ast::factory &ast, cog_la_tokenizer &tok)
    {
        if(tok.get_type() == cog_token_type::punc_apos) {
            // Vector literal
            auto start_pos = tok.get_location();
            tok.advance();

            float x = get_number<float>(tok);
            tok.advance();

            if(tok.get_type() == cog_token_type::punc_comma) {
                // Ignore comma
                tok.advance();
            }

            float y = get_number<float>(tok);
            tok.advance();

            if(tok.get_type() == cog_token_type::punc_comma) {
                // Ignore comma
                tok.advance();
            }

            float z = get_number<float>(tok);
            tok.advance();

            if(tok.get_type() != cog_token_type::punc_apos) {
                diagnostic_context dc(tok.get_location());
                LOG_FATAL(format("expected end of vector literal, found '%s'") % tok.get_value());
            }

            auto end_pos = tok.get_location();
            tok.advance();

            return ast.make_var<ast::expression, ast::vector_literal_expression>(
                    location_union(start_pos, end_pos), x, y, z);
        }
        else if(tok.get_type() == cog_token_type::integer ||
                tok.get_type() == cog_token_type::hex_integer) {
            ast::expression *rv = ast.make_var<ast::expression, ast::integer_literal_expression>(
                    tok.get_location(),
                    get_number<int>(tok));
            tok.advance();
            return rv;
        }
        else if(tok.get_type() == cog_token_type::real) {
            ast::expression *rv = ast.make_var<ast::expression, ast::float_literal_expression>(
                    tok.get_location(),
                    get_number<float>(tok));
            tok.advance();
            return rv;
        }
        else if(tok.get_type() == cog_token_type::string) {
            ast::expression *rv = ast.make_var<ast::expression, ast::string_literal_expression>(
                    tok.get_location(),
                    tok.get_value());
            tok.advance();
            return rv;
        }
        else {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected expression, found '%s'") % tok.get_value());
        }
    }

    ast::expression* parse_postfix_expression(ast::factory &ast, cog_la_tokenizer &tok)
    {
        if(tok.get_type() == cog_token_type::identifier) {
            // May be bare name, array index, or function call
            ast::identifier *ident = ast.make<ast::identifier>(tok.get_location(),
                                                               tok.get_value());
            tok.advance();

            if(tok.get_type() == cog_token_type::punc_lparen) {
                // Function call
                tok.advance();

                // Parse argument list
                ast::list_node<ast::expression *> *args =
                    ast.make<ast::list_node<ast::expression *>>(tok.get_location());

                if(tok.get_type() != cog_token_type::punc_rparen) {
                    // Argument list is not empty
                    while(true) {
                        args->elements.push_back(parse_assignment_expression(ast, tok));
                        args->location = location_union(args->location, tok.get_location());

                        if(tok.get_type() == cog_token_type::punc_comma) {
                            tok.advance();
                            continue;
                        }
                        else if(tok.get_type() == cog_token_type::punc_rparen) {
                            break;
                        }
                        else {
                            diagnostic_context dc(tok.get_location());
                            LOG_FATAL(format("expected ',' or ')', found '%s'") % tok.get_value());
                        }
                    }
                }

                ast::expression *rv = ast.make_var<ast::expression, ast::method_call_expression>(
                        location_union(ident->location, tok.get_location()),
                        ident,
                        args);

                tok.advance();
                return rv;
            }
            else if(tok.get_type() == cog_token_type::punc_lbracket) {
                // Array index
                tok.advance();

                ast::expression *subscript = parse_expression(ast, tok);

                if(tok.get_type() != cog_token_type::punc_rbracket) {
                    diagnostic_context dc(tok.get_location());
                    LOG_FATAL(format("expected ']', found '%s'") % tok.get_value());
                }

                auto end_loc = tok.get_location();
                tok.advance();

                return ast.make_var<ast::expression, ast::subscript_expression>(
                        location_union(ident->location, end_loc),
                        ident,
                        subscript);
            }
            else {
                return ast.make_var<ast::expression, ast::identifier_expression>(
                        ident->location,
                        ident);
            }
        }
        else if(tok.get_type() == cog_token_type::punc_lparen) {
            tok.advance();

            ast::expression *primary_expr = parse_expression(ast, tok);

            if(tok.get_type() != cog_token_type::punc_rparen) {
                diagnostic_context dc(tok.get_location());
                LOG_FATAL(format("expected ')', found '%s'") % tok.get_value());
            }
            tok.advance();

            return primary_expr;
        }
        else {
            return parse_literal_expression(ast, tok);
        }
    }

    ast::expression* parse_unary_expression(ast::factory &ast, cog_la_tokenizer &tok)
    {
        if(tok.get_type() == cog_token_type::punc_plus ||
           tok.get_type() == cog_token_type::punc_minus ||
           tok.get_type() == cog_token_type::punc_excl) {
            auto op = unary_operator_map(tok.get_type());
            auto start_loc = tok.get_location();
            tok.advance();

            ast::expression *rv = parse_unary_expression(ast, tok);

            return ast.make_var<ast::expression, ast::unary_expression>(
                    location_union(start_loc, ast::visit(variant_location_visitor(), *rv)),
                    rv,
                    op);
        }
        else {
            return parse_postfix_expression(ast, tok);
        }
    }

    bool multiplicative_operator_is_indicator(cog_token_type t)
    {
        return t == cog_token_type::punc_times ||
               t == cog_token_type::punc_div ||
               t == cog_token_type::punc_mod;
    }

    LEFT_ASSOCIATIVE_INFIX_EXPR(multiplicative_expression,
                                unary_expression,
                                multiplicative_operator_is_indicator)

    bool additive_operator_is_indicator(cog_token_type t)
    {
        return t == cog_token_type::punc_plus ||
               t == cog_token_type::punc_minus;
    }

    LEFT_ASSOCIATIVE_INFIX_EXPR(additive_expression,
                                multiplicative_expression,
                                additive_operator_is_indicator)

    bool relational_operator_is_indicator(cog_token_type t)
    {
        return t == cog_token_type::punc_greaterequal ||
               t == cog_token_type::punc_greater ||
               t == cog_token_type::punc_lessequal ||
               t == cog_token_type::punc_less;
    }

    LEFT_ASSOCIATIVE_INFIX_EXPR(relational_expression,
                                additive_expression,
                                relational_operator_is_indicator)

    bool equality_operator_is_indicator(cog_token_type t)
    {
        return t == cog_token_type::punc_equal ||
               t == cog_token_type::punc_notequal;
    }

    LEFT_ASSOCIATIVE_INFIX_EXPR(equality_expression,
                                relational_expression,
                                equality_operator_is_indicator)

    LEFT_ASSOCIATIVE_INFIX_EXPR_SIMPLE(and_expression,
                                       equality_expression,
                                       cog_token_type::punc_and)

    LEFT_ASSOCIATIVE_INFIX_EXPR_SIMPLE(exclusive_or_expression,
                                       and_expression,
                                       cog_token_type::punc_xor)

    LEFT_ASSOCIATIVE_INFIX_EXPR_SIMPLE(inclusive_or_expression,
                                       exclusive_or_expression,
                                       cog_token_type::punc_or)

    LEFT_ASSOCIATIVE_INFIX_EXPR_SIMPLE(logical_and_expression,
                                       inclusive_or_expression,
                                       cog_token_type::punc_logical_and)

    LEFT_ASSOCIATIVE_INFIX_EXPR_SIMPLE(logical_or_expression,
                                       logical_and_expression,
                                       cog_token_type::punc_logical_or)

    ast::expression* parse_assignment_expression(ast::factory &ast, cog_la_tokenizer &tok)
    {
        // Right associative
        ast::expression *lhs = parse_logical_or_expression(ast, tok);

        if(tok.get_type() == cog_token_type::punc_assign) {
            tok.advance();
            ast::expression *rhs = parse_assignment_expression(ast, tok);

            return ast.make_var<ast::expression, ast::assignment_expression>(
                    location_union(ast::visit(variant_location_visitor(), *lhs),
                                   ast::visit(variant_location_visitor(), *rhs)),
                    lhs,
                    rhs);
        }
        else {
            return lhs;
        }
    }

    ast::expression* parse_expression(ast::factory &ast, cog_la_tokenizer &tok)
    {
        // Left associative
        ast::expression *rv = parse_assignment_expression(ast, tok);
        while(tok.get_type() == cog_token_type::punc_comma) {
            tok.advance();
            ast::expression *right = parse_assignment_expression(ast, tok);
            rv = ast.make_var<ast::expression, ast::comma_expression>(
                    location_union(ast::visit(variant_location_visitor(), *rv),
                                   ast::visit(variant_location_visitor(), *right)),
                    rv,
                    right);
        }

        return rv;
    }

    // Code:

    ast::statement* parse_statement(ast::factory &, cog_la_tokenizer &);

    ast::statement* parse_expression_statement(ast::factory &ast, cog_la_tokenizer &tok)
    {
        ast::expression *value = parse_expression(ast, tok);

        if(tok.get_type() != cog_token_type::punc_semicolon) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ';', found '%s'") % tok.get_value());
        }

        auto end_loc = tok.get_location();
        tok.advance();

        return ast.make_var<ast::statement, ast::expression_statement>(
                location_union(ast::visit(variant_location_visitor(), *value), end_loc),
                value);
    }

    ast::statement* parse_break_statement(ast::factory &ast, cog_la_tokenizer &tok)
    {
        auto start_pos = tok.get_location();
        tok.advance();

        if(tok.get_type() != cog_token_type::punc_semicolon) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ';', found '%s'") % tok.get_value());
        }

        auto end_pos = tok.get_location();
        tok.advance();

        return ast.make_var<ast::statement, ast::break_statement>(
                location_union(start_pos, end_pos));
    }

    ast::statement* parse_return_statement(ast::factory &ast, cog_la_tokenizer &tok)
    {
        auto start_pos = tok.get_location();
        tok.advance();

        if(tok.get_type() != cog_token_type::punc_semicolon) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ';', found '%s'") % tok.get_value());
        }

        auto end_pos = tok.get_location();
        tok.advance();

        return ast.make_var<ast::statement, ast::return_statement>(
                location_union(start_pos, end_pos));
    }

    ast::statement* parse_call_statement(ast::factory &ast, cog_la_tokenizer &tok)
    {
        auto start_pos = tok.get_location();
        tok.advance();

        if(tok.get_type() != cog_token_type::identifier) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected destination label, found '%s'") % tok.get_value());
        }

        ast::identifier *target = ast.make<ast::identifier>(tok.get_location(), tok.get_value());
        tok.advance();

        if(tok.get_type() != cog_token_type::punc_semicolon) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ';', found '%s'") % tok.get_value());
        }

        auto end_pos = tok.get_location();
        tok.advance();

        return ast.make_var<ast::statement, ast::call_statement>(
                location_union(start_pos, end_pos), target);
    }

    ast::statement* parse_if_statement(ast::factory &ast, cog_la_tokenizer &tok)
    {
        auto start_pos = tok.get_location();
        tok.advance();

        if(tok.get_type() != cog_token_type::punc_lparen) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected '(', found '%s'") % tok.get_value());
        }
        tok.advance();

        ast::expression *cond = parse_expression(ast, tok);

        if(tok.get_type() != cog_token_type::punc_rparen) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ')', found '%s'") % tok.get_value());
        }
        tok.advance();

        ast::statement *code = parse_statement(ast, tok);

        if(!iequal(tok.get_value(), "else"_sv)) {
            // if statement is not followed by an else term
            return ast.make_var<ast::statement, ast::if_statement>(
                    location_union(start_pos,
                                   ast::visit(variant_location_visitor(), *code)),
                    cond,
                    code);
        }

        // if statement is followed by an else term
        tok.advance();
        ast::statement *else_code = parse_statement(ast, tok);

        return ast.make_var<ast::statement, ast::if_else_statement>(
                location_union(start_pos,
                               ast::visit(variant_location_visitor(), *else_code)),
                cond,
                code,
                else_code);
    }

    ast::statement* parse_while_statement(ast::factory &ast, cog_la_tokenizer &tok)
    {
        auto start_pos = tok.get_location();
        tok.advance();

        if(tok.get_type() != cog_token_type::punc_lparen) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected '(', found '%s'") % tok.get_value());
        }
        tok.advance();

        ast::expression *cond = parse_expression(ast, tok);

        if(tok.get_type() != cog_token_type::punc_rparen) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ')', found '%s'") % tok.get_value());
        }
        tok.advance();

        ast::statement *code = parse_statement(ast, tok);

        return ast.make_var<ast::statement, ast::while_statement>(
                location_union(start_pos,
                               ast::visit(variant_location_visitor(), *code)),
                cond,
                code);
    }

    ast::statement* parse_do_statement(ast::factory &ast, cog_la_tokenizer &tok)
    {
        auto start_pos = tok.get_location();
        tok.advance();

        ast::statement *code = parse_statement(ast, tok);

        if(!iequal(tok.get_value(), "while"_sv)) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected 'while', found '%s'") % tok.get_value());
        }
        tok.advance();

        if(tok.get_type() != cog_token_type::punc_lparen) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected '(', found '%s'") % tok.get_value());
        }
        tok.advance();

        ast::expression *cond = parse_expression(ast, tok);

        if(tok.get_type() != cog_token_type::punc_rparen) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ')', found '%s'") % tok.get_value());
        }
        tok.advance();

        if(tok.get_type() != cog_token_type::punc_semicolon) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ';', found '%s'") % tok.get_value());
        }

        auto end_pos = tok.get_location();
        tok.advance();

        return ast.make_var<ast::statement, ast::do_statement>(
                location_union(start_pos, end_pos),
                code,
                cond);
    }

    ast::statement* parse_for_statement(ast::factory &ast, cog_la_tokenizer &tok)
    {
        auto start_pos = tok.get_location();
        tok.advance();

        if(tok.get_type() != cog_token_type::punc_lparen) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected '(', found '%s'") % tok.get_value());
        }
        tok.advance();

        ast::for_optional_expression *init;
        if(tok.get_type() == cog_token_type::punc_semicolon) {
            init = ast.make_var<ast::for_optional_expression, ast::for_empty_expression>(
                    tok.get_location());
        }
        else {
            ast::expression *sub_expr = parse_expression(ast, tok);

            init = ast.make_var<ast::for_optional_expression, ast::for_expression>(
                    ast::visit(variant_location_visitor(), *sub_expr),
                    sub_expr);
        }

        if(tok.get_type() != cog_token_type::punc_semicolon) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ';', found '%s'") % tok.get_value());
        }
        tok.advance();

        ast::for_optional_expression *cond;
        if(tok.get_type() == cog_token_type::punc_semicolon) {
            cond = ast.make_var<ast::for_optional_expression, ast::for_empty_expression>(
                    tok.get_location());
        }
        else {
            ast::expression *sub_expr = parse_expression(ast, tok);

            cond = ast.make_var<ast::for_optional_expression, ast::for_expression>(
                    ast::visit(variant_location_visitor(), *sub_expr),
                    sub_expr);
        }

        if(tok.get_type() != cog_token_type::punc_semicolon) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ';', found '%s'") % tok.get_value());
        }
        tok.advance();

        ast::for_optional_expression *iter;
        if(tok.get_type() == cog_token_type::punc_rparen) {
            iter = ast.make_var<ast::for_optional_expression, ast::for_empty_expression>(
                    tok.get_location());
        }
        else {
            ast::expression *sub_expr = parse_expression(ast, tok);

            iter = ast.make_var<ast::for_optional_expression, ast::for_expression>(
                    ast::visit(variant_location_visitor(), *sub_expr),
                    sub_expr);
        }

        if(tok.get_type() != cog_token_type::punc_rparen) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected ')', found '%s'") % tok.get_value());
        }
        tok.advance();

        ast::statement *code = parse_statement(ast, tok);

        return ast.make_var<ast::statement, ast::for_statement>(
                location_union(start_pos,
                               ast::visit(variant_location_visitor(), *code)),
                init,
                cond,
                iter,
                code);
    }

    ast::statement* parse_labeled_statement(ast::factory &ast, cog_la_tokenizer &tok)
    {
        auto start_pos = tok.get_location();
        ast::identifier *label = ast.make<ast::identifier>(start_pos, tok.get_value());
        tok.advance();

        // Already know the next token is a colon, due to lookahead
        tok.advance();

        ast::statement *stmt = parse_statement(ast, tok);

        return ast.make_var<ast::statement, ast::labeled_statement>(
                location_union(start_pos,
                               ast::visit(variant_location_visitor(), *stmt)),
                label,
                stmt);
    }

    ast::statement* parse_leading_identifier_statement(ast::factory &ast, cog_la_tokenizer &tok)
    {
        if(iequal(tok.get_value(), "break"_sv)) {
            return parse_break_statement(ast, tok);
        }
        else if(iequal(tok.get_value(), "return"_sv)) {
            return parse_return_statement(ast, tok);
        }
        else if(iequal(tok.get_value(), "call"_sv)) {
            return parse_call_statement(ast, tok);
        }
        else if(iequal(tok.get_value(), "if"_sv)) {
            return parse_if_statement(ast, tok);
        }
        else if(iequal(tok.get_value(), "while"_sv)) {
            return parse_while_statement(ast, tok);
        }
        else if(iequal(tok.get_value(), "do"_sv)) {
            return parse_do_statement(ast, tok);
        }
        else if(iequal(tok.get_value(), "for"_sv)) {
            return parse_for_statement(ast, tok);
        }
        else if(tok.get_token(1).type == cog_token_type::punc_colon) {
            return parse_labeled_statement(ast, tok);
        }
        else {
            return parse_expression_statement(ast, tok);
        }
    }

    ast::statement* parse_compound_statement(ast::factory &ast, cog_la_tokenizer &tok)
    {
        // Starts on left brace
        auto start_loc = tok.get_location();
        tok.advance();

        ast::list_node<ast::statement*> *code =
            ast.make<ast::list_node<ast::statement*>>(start_loc);

        while(true) {
            if(tok.get_type() == cog_token_type::end_of_file) {
                diagnostic_context dc(tok.get_location());
                LOG_FATAL(format("unexpected end of file in compound statement"));
            }
            else if(tok.get_type() == cog_token_type::punc_rbrace) {
                break;
            }

            code->elements.push_back(parse_statement(ast, tok));
            code->location = location_union(code->location,
                                            ast::visit(variant_location_visitor(),
                                                       *code->elements.back()));
        }

        auto end_loc = tok.get_location();
        tok.advance();

        return ast.make_var<ast::statement, ast::compound_statement>(
                location_union(start_loc, end_loc), code);
    }

    ast::statement* parse_statement(ast::factory &ast, cog_la_tokenizer &tok)
    {
        if(tok.get_type() == cog_token_type::identifier) {
            // Leads with identifier or keyword
            return parse_leading_identifier_statement(ast, tok);
        }
        else if(tok.get_type() == cog_token_type::punc_lbrace) {
            // Compound statement
            return parse_compound_statement(ast, tok);
        }
        else if(tok.get_type() == cog_token_type::punc_semicolon) {
            auto location = tok.get_location();
            tok.advance();
            return ast.make_var<ast::statement, ast::empty_statement>(location);
        }
        else {
            return parse_expression_statement(ast, tok);
        }
    }

    ast::code_section* parse_code_section(ast::factory &ast, cog_la_tokenizer &tok)
    {
        if(!iequal(tok.get_value(), "code"_sv)) {
            diagnostic_context dc(tok.get_location());
            LOG_FATAL(format("expected 'code', found '%s'") % tok.get_value());
        }

        auto start_loc = tok.get_location();
        tok.advance();

        ast::list_node<ast::statement*> *code =
            ast.make<ast::list_node<ast::statement*>>(start_loc);

        while(true) {
            if(tok.get_type() == cog_token_type::end_of_file) {
                diagnostic_context dc(tok.get_location());
                LOG_FATAL(format("unexpected end of file in code section"));
            }
            else if(iequal(tok.get_value(), "end"_sv)) {
                break;
            }

            code->elements.push_back(parse_statement(ast, tok));
            code->location = location_union(code->location,
                                            ast::visit(variant_location_visitor(),
                                                       *code->elements.back()));
        }

        auto end_loc = tok.get_location();
        tok.advance();

        return ast.make<ast::code_section>(location_union(start_loc, end_loc), code);
    }

    // Translation unit:

    ast::flags_section* parse_flags_section(ast::factory &factory, cog_tokenizer &tok)
    {
        if(!iequal(tok.get_value(), "flags"_sv)) {
            // No flags section
            return factory.make<ast::flags_section>(diagnostic_context_location(nothing, 0, 0), 0);
        }

        auto start_loc = tok.get_location();
        tok.advance();

        if(tok.get_type() != cog_token_type::punc_assign) {
            diagnostic_context_location dc(tok.get_location());
            LOG_FATAL(format("expected '=', found '%s'") % tok.get_value());
        }

        tok.advance();

        unsigned int flags = get_number<unsigned int>(tok);

        auto end_loc = tok.get_location();
        tok.advance();

        return factory.make<ast::flags_section>(location_union(start_loc, end_loc),
                                                flags);
    }

    ast::translation_unit* parse_translation_unit(ast::factory &factory, cog_tokenizer &tok)
    {
        // Symbols section requires one-token lookahead with string fragment hacks.
        // Don't construct lookahead tokenizer until after.
        ast::flags_section *flags = parse_flags_section(factory, tok);
        ast::symbols_section *symbols = parse_symbols_section(factory, tok);

        // Code section requires two-token lookahead.
        // This poisons the original, one-lookahead tokenizer.
        cog_la_tokenizer latok(tok, 2);

        ast::code_section *code = parse_code_section(factory, latok);

        return factory.make<ast::translation_unit>(location_union(flags->location,
                                                                  code->location),
                                                   flags,
                                                   symbols,
                                                   code);
    }

}

ast::translation_unit* gorc::cog::parse_cog(ast::factory &factory, cog_tokenizer &tok)
{
    return parse_translation_unit(factory, tok);
}
