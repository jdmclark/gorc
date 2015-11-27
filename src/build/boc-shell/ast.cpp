#include "ast.hpp"

gorc::word::word(diagnostic_context_location const &loc,
                 std::string const &v)
    : visitable_ast_node(loc)
    , value(v)
{
    return;
}

gorc::command_statement::command_statement(diagnostic_context_location const &loc,
                                           ast_list_node<word*> *arguments)
    : visitable_ast_node(loc)
    , arguments(arguments)
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
