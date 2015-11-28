#include "ast.hpp"

gorc::simple_word::simple_word(diagnostic_context_location const &loc,
                               std::string const &v)
    : visitable_ast_node(loc)
    , value(v)
{
    return;
}

gorc::variable_name::variable_name(diagnostic_context_location const &loc,
                                   std::string const &name)
    : visitable_ast_node(loc)
    , name(name)
{
    return;
}

gorc::argument::argument(diagnostic_context_location const &loc,
                         ast_list_node<word*> *words)
    : visitable_ast_node(loc)
    , words(words)
{
    return;
}

gorc::subcommand::subcommand(diagnostic_context_location const &loc,
                             ast_list_node<argument*> *arguments)
    : visitable_ast_node(loc)
    , arguments(arguments)
{
    return;
}

gorc::pipe_command::pipe_command(diagnostic_context_location const &loc,
                                 ast_list_node<subcommand*> *subcommands)
    : visitable_ast_node(loc)
    , subcommands(subcommands)
{
    return;
}

gorc::command_statement::command_statement(diagnostic_context_location const &loc,
                                           command *cmd)
    : visitable_ast_node(loc)
    , cmd(cmd)
{
    return;
}

gorc::assignment_statement::assignment_statement(diagnostic_context_location const &loc,
                                                 variable_name *var,
                                                 argument *value)
    : visitable_ast_node(loc)
    , var(var)
    , value(value)
{
    return;
}

gorc::export_statement::export_statement(diagnostic_context_location const &loc,
                                         variable_name *var)
    : visitable_ast_node(loc)
    , var(var)
{
    return;
}

gorc::translation_unit::translation_unit(diagnostic_context_location const &loc,
                                         ast_list_node<statement*> *code)
    : visitable_ast_node(loc)
    , code(code)
{
    return;
}
