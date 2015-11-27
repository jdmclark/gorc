#pragma once

#include "ast/node.hpp"
#include <string>

namespace gorc {

    /* General */

    class word : public visitable_ast_node<word> {
    public:
        std::string value;

        word(diagnostic_context_location const &loc,
             std::string const &value);
    };

    /* Statements */

    class command_statement;
    using statement = variant<command_statement*>;

    class command_statement : public visitable_ast_node<command_statement> {
    public:
        ast_list_node<word*> *arguments;

        command_statement(diagnostic_context_location const &loc,
                          ast_list_node<word*> *arguments);
    };

    class translation_unit : public visitable_ast_node<translation_unit> {
    public:
        ast_list_node<statement*> *code;

        translation_unit(diagnostic_context_location const &loc,
                         ast_list_node<statement*> *code);
    };

}
