#pragma once

#include "ast/node.hpp"
#include "cog/script/value.hpp"
#include <string>

namespace gorc {
    namespace cog {
        namespace ast {

            enum class unary_operator {
                plus,
                minus,
                logical_not
            };

            enum class infix_operator {
                addition,
                subtraction,
                multiplication,
                division,
                modulo,

                greater,
                greater_equal,
                less,
                less_equal,
                equal,
                not_equal,

                bitwise_and,
                bitwise_or,
                bitwise_xor,

                logical_and,
                logical_or
            };

            /* General */

            class identifier : public visitable_ast_node<identifier> {
            public:
                std::string value;

                identifier(diagnostic_context_location const &loc,
                           std::string const &value);
            };

            /* Symbol section */

            class string_fragment_field : public visitable_ast_node<string_fragment_field> {
            public:
                std::string value;

                string_fragment_field(diagnostic_context_location const &loc,
                                      std::string const &value);
            };

            class integer_field : public visitable_ast_node<integer_field> {
            public:
                int value;

                integer_field(diagnostic_context_location const &loc,
                              int value);
            };

            class float_field : public visitable_ast_node<float_field> {
            public:
                float value;

                float_field(diagnostic_context_location const &loc,
                            float value);
            };

            using symbol_field = variant<string_fragment_field*,
                                         integer_field*,
                                         float_field*>;

            class bare_extension : public visitable_ast_node<bare_extension> {
            public:
                identifier *name;

                bare_extension(diagnostic_context_location const &loc,
                               identifier *name);
            };

            class valued_extension : public visitable_ast_node<valued_extension> {
            public:
                identifier *name;
                symbol_field *value;

                valued_extension(diagnostic_context_location const &loc,
                                 identifier *name,
                                 symbol_field *value);
            };

            using symbol_extension = variant<bare_extension*,
                                             valued_extension*>;

            class symbol : public visitable_ast_node<symbol> {
            public:
                identifier *type;
                identifier *name;
                ast_list_node<symbol_extension*> *extensions;
                maybe<symbol_field*> value;

                symbol(diagnostic_context_location const &loc,
                       identifier *type,
                       identifier *name,
                       ast_list_node<symbol_extension*> *extensions,
                       maybe<symbol_field*> value = nothing);
            };

            /* Expressions */
            class immediate_expression;
            class string_literal_expression;
            class integer_literal_expression;
            class float_literal_expression;
            class vector_literal_expression;
            class identifier_expression;
            class subscript_expression;
            class method_call_expression;
            class unary_expression;
            class infix_expression;
            class assignment_expression;
            class comma_expression;
            using expression = variant<immediate_expression*,
                                       string_literal_expression*,
                                       integer_literal_expression*,
                                       float_literal_expression*,
                                       vector_literal_expression*,
                                       identifier_expression*,
                                       subscript_expression*,
                                       method_call_expression*,
                                       unary_expression*,
                                       infix_expression*,
                                       assignment_expression*,
                                       comma_expression*>;

            class immediate_expression : public visitable_ast_node<immediate_expression> {
            public:
                cog::value value;

                immediate_expression(diagnostic_context_location const &loc,
                                     cog::value value);
            };

            class string_literal_expression : public visitable_ast_node<string_literal_expression> {
            public:
                std::string value;

                string_literal_expression(diagnostic_context_location const &loc,
                                          std::string const &value);
            };

            class integer_literal_expression : public visitable_ast_node<integer_literal_expression> {
            public:
                int value;

                integer_literal_expression(diagnostic_context_location const &loc,
                                           int value);
            };

            class float_literal_expression : public visitable_ast_node<float_literal_expression> {
            public:
                float value;

                float_literal_expression(diagnostic_context_location const &loc,
                                         float value);
            };

            class vector_literal_expression : public visitable_ast_node<vector_literal_expression> {
            public:
                float x, y, z;

                vector_literal_expression(diagnostic_context_location const &loc,
                                          float x,
                                          float y,
                                          float z);
            };

            class identifier_expression : public visitable_ast_node<identifier_expression> {
            public:
                identifier *value;

                identifier_expression(diagnostic_context_location const &loc,
                                      identifier *value);
            };

            class subscript_expression : public visitable_ast_node<subscript_expression> {
            public:
                identifier *base;
                expression *index;

                subscript_expression(diagnostic_context_location const &loc,
                                     identifier *base,
                                     expression *index);
            };

            class method_call_expression : public visitable_ast_node<method_call_expression> {
            public:
                identifier *base;
                ast_list_node<expression*> *arguments;

                method_call_expression(diagnostic_context_location const &loc,
                                       identifier *base,
                                       ast_list_node<expression*> *arguments);
            };

            class unary_expression : public visitable_ast_node<unary_expression> {
            public:
                expression *base;
                unary_operator op;

                unary_expression(diagnostic_context_location const &loc,
                                 expression *base,
                                 unary_operator op);
            };

            class infix_expression : public visitable_ast_node<infix_expression> {
            public:
                expression *left;
                expression *right;
                infix_operator op;

                infix_expression(diagnostic_context_location const &loc,
                                 expression *left,
                                 expression *right,
                                 infix_operator op);
            };

            class assignment_expression : public visitable_ast_node<assignment_expression> {
            public:
                expression *target;
                expression *value;

                assignment_expression(diagnostic_context_location const &loc,
                                      expression *target,
                                      expression *value);
            };

            class comma_expression : public visitable_ast_node<comma_expression> {
            public:
                expression *left;
                expression *right;

                comma_expression(diagnostic_context_location const &loc,
                                 expression *left,
                                 expression *right);
            };

            /* Statements */

            class compound_statement;
            class empty_statement;
            class expression_statement;
            class break_statement;
            class return_statement;
            class call_statement;
            class if_statement;
            class if_else_statement;
            class while_statement;
            class do_statement;
            class for_statement;
            class labeled_statement;
            using statement = variant<compound_statement*,
                                      empty_statement*,
                                      expression_statement*,
                                      break_statement*,
                                      return_statement*,
                                      call_statement*,
                                      if_statement*,
                                      if_else_statement*,
                                      while_statement*,
                                      do_statement*,
                                      for_statement*,
                                      labeled_statement*>;

            class compound_statement : public visitable_ast_node<compound_statement> {
            public:
                ast_list_node<statement*> *code;

                compound_statement(diagnostic_context_location const &loc,
                                   ast_list_node<statement*> *code);
            };

            class empty_statement : public visitable_ast_node<empty_statement> {
            public:
                empty_statement(diagnostic_context_location const &loc);
            };

            class expression_statement : public visitable_ast_node<expression_statement> {
            public:
                expression *value;

                expression_statement(diagnostic_context_location const &loc,
                                     expression *value);
            };

            class break_statement : public visitable_ast_node<break_statement> {
            public:
                break_statement(diagnostic_context_location const &loc);
            };

            class return_statement : public visitable_ast_node<return_statement> {
            public:
                return_statement(diagnostic_context_location const &loc);
            };

            class call_statement : public visitable_ast_node<call_statement> {
            public:
                identifier *label;

                call_statement(diagnostic_context_location const &loc,
                               identifier *label);
            };

            class if_statement : public visitable_ast_node<if_statement> {
            public:
                expression *condition;
                statement *code;

                if_statement(diagnostic_context_location const &loc,
                             expression *condition,
                             statement *code);
            };

            class if_else_statement : public visitable_ast_node<if_else_statement> {
            public:
                expression *condition;
                statement *code;
                statement *else_code;

                if_else_statement(diagnostic_context_location const &loc,
                                  expression *condition,
                                  statement *code,
                                  statement *else_code);
            };

            class while_statement : public visitable_ast_node<while_statement> {
            public:
                expression *condition;
                statement *code;

                while_statement(diagnostic_context_location const &loc,
                                expression *condition,
                                statement *code);
            };

            class do_statement : public visitable_ast_node<do_statement> {
            public:
                statement *code;
                expression *condition;

                do_statement(diagnostic_context_location const &loc,
                             statement *code,
                             expression *condition);
            };

            class for_empty_expression : public visitable_ast_node<for_empty_expression> {
            public:
                for_empty_expression(diagnostic_context_location const &loc);
            };

            class for_expression : public visitable_ast_node<for_expression> {
            public:
                expression *condition;

                for_expression(diagnostic_context_location const &loc,
                               expression *condition);
            };

            using for_optional_expression = variant<for_empty_expression*,
                                                    for_expression*>;

            class for_statement : public visitable_ast_node<for_statement> {
            public:
                for_optional_expression *initializer;
                for_optional_expression *condition;
                for_optional_expression *incrementer;
                statement *code;

                for_statement(diagnostic_context_location const &loc,
                              for_optional_expression *initializer,
                              for_optional_expression *condition,
                              for_optional_expression *incrementer,
                              statement *code);
            };

            class labeled_statement : public visitable_ast_node<labeled_statement> {
            public:
                identifier *label;
                statement *code;

                labeled_statement(diagnostic_context_location const &loc,
                                  identifier *label,
                                  statement *code);
            };

            /* Translation unit */

            class flags_section : public visitable_ast_node<flags_section> {
            public:
                unsigned int flags;

                flags_section(diagnostic_context_location const &loc,
                              unsigned int flags);
            };

            class symbols_section : public visitable_ast_node<symbols_section> {
            public:
                ast_list_node<symbol*> *symbols;

                symbols_section(diagnostic_context_location const &loc,
                                ast_list_node<symbol*> *symbols);
            };

            class code_section : public visitable_ast_node<code_section> {
            public:
                ast_list_node<statement*> *code;

                code_section(diagnostic_context_location const &loc,
                             ast_list_node<statement*> *code);
            };

            class translation_unit : public visitable_ast_node<translation_unit> {
            public:
                flags_section *flags;
                symbols_section *symbols;
                code_section *code;

                translation_unit(diagnostic_context_location const &loc,
                                 flags_section *flags,
                                 symbols_section *symbols,
                                 code_section *code);
            };
        }
    }
}
