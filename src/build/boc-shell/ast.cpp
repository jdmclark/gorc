#include "ast.hpp"

gorc::argument_expression::argument_expression(diagnostic_context_location const &loc,
                                               argument *value)
    : visitable_ast_node(loc)
    , value(value)
{
    return;
}

gorc::nil_expression::nil_expression(diagnostic_context_location const &loc)
    : visitable_ast_node(loc)
{
    return;
}

gorc::unary_expression::unary_expression(diagnostic_context_location const &loc,
                                         expression *value,
                                         unary_operator op)
    : visitable_ast_node(loc)
    , value(value)
    , op(op)
{
    return;
}

gorc::infix_expression::infix_expression(diagnostic_context_location const &loc,
                                         expression *left,
                                         expression *right,
                                         infix_operator op)
    : visitable_ast_node(loc)
    , left(left)
    , right(right)
    , op(op)
{
    return;
}

gorc::simple_word::simple_word(diagnostic_context_location const &loc,
                               std::string const &v)
    : visitable_ast_node(loc)
    , value(v)
{
    return;
}

gorc::expression_word::expression_word(diagnostic_context_location const &loc,
                                       expression *value)
    : visitable_ast_node(loc)
    , value(value)
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

gorc::environment_variable_name::environment_variable_name(diagnostic_context_location const &loc,
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

gorc::compound_statement::compound_statement(diagnostic_context_location const &loc,
                                             ast_list_node<statement*> *code)
    : visitable_ast_node(loc)
    , code(code)
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

gorc::var_declaration_statement::var_declaration_statement(diagnostic_context_location const &loc,
                                                           variable_name *var,
                                                           maybe<ast_list_node<argument*>*> value)
    : visitable_ast_node(loc)
    , var(var)
    , value(value)
{
    return;
}

gorc::assignment_statement::assignment_statement(diagnostic_context_location const &loc,
                                                 lvalue *var,
                                                 ast_list_node<argument*> *value)
    : visitable_ast_node(loc)
    , var(var)
    , value(value)
{
    return;
}

gorc::if_statement::if_statement(diagnostic_context_location const &loc,
                                 expression *condition,
                                 statement *code)
    : visitable_ast_node(loc)
    , condition(condition)
    , code(code)
{
    return;
}

gorc::if_else_statement::if_else_statement(diagnostic_context_location const &loc,
                                           expression *condition,
                                           statement *code,
                                           statement *elsecode)
    : visitable_ast_node(loc)
    , condition(condition)
    , code(code)
    , elsecode(elsecode)
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
