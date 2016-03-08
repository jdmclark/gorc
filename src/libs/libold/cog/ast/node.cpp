#include "node.hpp"

using namespace gorc::cog::ast;

node::node(diagnostic_context_location const &loc)
    : location(loc)
{
    return;
}

node::~node()
{
    return;
}

string_fragment_field::string_fragment_field(diagnostic_context_location const &loc,
                                             std::string const &value)
    : visitable_node(loc)
    , value(value)
{
    return;
}

integer_field::integer_field(diagnostic_context_location const &loc,
                             int value)
    : visitable_node(loc)
    , value(value)
{
    return;
}

float_field::float_field(diagnostic_context_location const &loc,
                         float value)
    : visitable_node(loc)
    , value(value)
{
    return;
}

bare_extension::bare_extension(diagnostic_context_location const &loc,
                               std::string const &name)
    : visitable_node(loc)
    , name(name)
{
    return;
}

valued_extension::valued_extension(diagnostic_context_location const &loc,
                                   std::string const &name,
                                   symbol_field *value)
    : visitable_node(loc)
    , name(name)
    , value(value)
{
    return;
}

symbol::symbol(diagnostic_context_location const &loc,
               std::string const &type,
               std::string const &name,
               std::vector<symbol_extension*>* extensions,
               maybe<symbol_field*> value)
    : visitable_node(loc)
    , type(type)
    , name(name)
    , extensions(extensions)
    , value(value)
{
    return;
}

string_literal_expression::string_literal_expression(diagnostic_context_location const &loc,
                                                     std::string const &value)
    : visitable_node(loc)
    , value(value)
{
    return;
}

integer_literal_expression::integer_literal_expression(diagnostic_context_location const &loc,
                                                       int value)
    : visitable_node(loc)
    , value(value)
{
    return;
}

float_literal_expression::float_literal_expression(diagnostic_context_location const &loc,
                                                   float value)
    : visitable_node(loc)
    , value(value)
{
    return;
}

vector_literal_expression::vector_literal_expression(diagnostic_context_location const &loc,
                                                     float x,
                                                     float y,
                                                     float z)
    : visitable_node(loc)
    , x(x)
    , y(y)
    , z(z)
{
    return;
}

identifier_expression::identifier_expression(diagnostic_context_location const &loc,
                                             std::string const &identifier)
    : visitable_node(loc)
    , identifier(identifier)
{
    return;
}

subscript_expression::subscript_expression(diagnostic_context_location const &loc,
                                           std::string const &base,
                                           expression *index)
    : visitable_node(loc)
    , base(base)
    , index(index)
{
    return;
}

method_call_expression::method_call_expression(diagnostic_context_location const &loc,
                                               std::string const &base,
                                               std::vector<expression*>* arguments)
    : visitable_node(loc)
    , base(base)
    , arguments(arguments)
{
    return;
}

unary_expression::unary_expression(diagnostic_context_location const &loc,
                                   expression *base,
                                   unary_operator op)
    : visitable_node(loc)
    , base(base)
    , op(op)
{
    return;
}

infix_expression::infix_expression(diagnostic_context_location const &loc,
                                   expression *left,
                                   expression *right,
                                   infix_operator op)
    : visitable_node(loc)
    , left(left)
    , right(right)
    , op(op)
{
    return;
}

assignment_expression::assignment_expression(diagnostic_context_location const &loc,
                                             expression *target,
                                             expression *value)
    : visitable_node(loc)
    , target(target)
    , value(value)
{
    return;
}

comma_expression::comma_expression(diagnostic_context_location const &loc,
                                   expression *left,
                                   expression *right)
    : visitable_node(loc)
    , left(left)
    , right(right)
{
    return;
}

compound_statement::compound_statement(diagnostic_context_location const &loc,
                                       std::vector<statement*>* code)
    : visitable_node(loc)
    , code(code)
{
    return;
}

empty_statement::empty_statement(diagnostic_context_location const &loc)
    : visitable_node(loc)
{
    return;
}

expression_statement::expression_statement(diagnostic_context_location const &loc,
                                           expression *expr)
    : visitable_node(loc)
    , expr(expr)
{
    return;
}

break_statement::break_statement(diagnostic_context_location const &loc)
    : visitable_node(loc)
{
    return;
}

return_statement::return_statement(diagnostic_context_location const &loc)
    : visitable_node(loc)
{
    return;
}

call_statement::call_statement(diagnostic_context_location const &loc,
                               std::string const &label)
    : visitable_node(loc)
    , label(label)
{
    return;
}

if_statement::if_statement(diagnostic_context_location const &loc,
                           expression *condition,
                           statement *code)
    : visitable_node(loc)
    , condition(condition)
    , code(code)
{
    return;
}

if_else_statement::if_else_statement(diagnostic_context_location const &loc,
                                     expression *condition,
                                     statement *code,
                                     statement *else_code)
    : visitable_node(loc)
    , condition(condition)
    , code(code)
    , else_code(else_code)
{
    return;
}

while_statement::while_statement(diagnostic_context_location const &loc,
                                 expression *condition,
                                 statement *code)
    : visitable_node(loc)
    , condition(condition)
    , code(code)
{
    return;
}

do_statement::do_statement(diagnostic_context_location const &loc,
                           statement *code,
                           expression *condition)
    : visitable_node(loc)
    , code(code)
    , condition(condition)
{
    return;
}

for_empty_expression::for_empty_expression(diagnostic_context_location const &loc)
    : visitable_node(loc)
{
    return;
}

for_expression::for_expression(diagnostic_context_location const &loc,
                               expression *condition)
    : visitable_node(loc)
    , condition(condition)
{
    return;
}

for_statement::for_statement(diagnostic_context_location const &loc,
                             for_optional_expression *initializer,
                             for_optional_expression *condition,
                             for_optional_expression *incrementer,
                             statement *code)
    : visitable_node(loc)
    , initializer(initializer)
    , condition(condition)
    , incrementer(incrementer)
    , code(code)
{
    return;
}

labeled_statement::labeled_statement(diagnostic_context_location const &loc,
                                     std::string const &label,
                                     statement *code)
    : visitable_node(loc)
    , label(label)
    , code(code)
{
    return;
}

translation_unit::translation_unit(diagnostic_context_location const &loc,
                                   unsigned int flags,
                                   std::vector<symbol*>* symbols,
                                   std::vector<statement*>* code)
    : visitable_node(loc)
    , flags(flags)
    , symbols(symbols)
    , code(code)
{
    return;
}
