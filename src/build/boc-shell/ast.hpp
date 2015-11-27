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

    class argument : public visitable_ast_node<argument> {
    public:
        ast_list_node<word*> *words;

        argument(diagnostic_context_location const &loc,
                 ast_list_node<word*> *words);
    };

    /* Commands */

    class subcommand : public visitable_ast_node<subcommand> {
    public:
        ast_list_node<argument*> *arguments;

        subcommand(diagnostic_context_location const &loc,
                   ast_list_node<argument*> *arguments);
    };

    class pipe_command;
    using command = variant<pipe_command*>;

    class pipe_command : public visitable_ast_node<pipe_command> {
    public:
        ast_list_node<subcommand*> *subcommands;

        pipe_command(diagnostic_context_location const &loc,
                     ast_list_node<subcommand*> *subcommands);
    };

    /* Statements */

    class command_statement;
    using statement = variant<command_statement*>;

    class command_statement : public visitable_ast_node<command_statement> {
    public:
        command *cmd;

        command_statement(diagnostic_context_location const &loc,
                          command *cmd);
    };

    class translation_unit : public visitable_ast_node<translation_unit> {
    public:
        ast_list_node<statement*> *code;

        translation_unit(diagnostic_context_location const &loc,
                         ast_list_node<statement*> *code);
    };

}
