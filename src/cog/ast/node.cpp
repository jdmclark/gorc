#include "node.h"
#include "visitor.h"

using namespace gorc::cog::ast;

node::node() {
    return;
}

node::~node() {
    return;
}

string_fragment_field::string_fragment_field(const std::string& value) : value(value) {
    return;
}

integer_field::integer_field(int value) : value(value) {
    return;
}

float_field::float_field(float value) : value(value) {
    return;
}

bare_extension::bare_extension(const std::string& name) : name(name) {
    return;
}

valued_extension::valued_extension(const std::string& name, symbol_field* value) : bare_extension(name), value(value) {
    return;
}

symbol::symbol(const std::string& type, const std::string& name, std::vector<symbol_extension*>* extensions)
    : type(type), name(name), extensions(extensions) {
    return;
}

valued_symbol::valued_symbol(const std::string& type, const std::string& name, std::vector<symbol_extension*>* extensions, symbol_field* value)
    : symbol(type, name, extensions), value(value) {
    return;
}

string_literal_expression::string_literal_expression(const std::string& value) : value(value) {
    return;
}

integer_literal_expression::integer_literal_expression(int value) : value(value) {
    return;
}

float_literal_expression::float_literal_expression(float value) : value(value) {
    return;
}

vector_literal_expression::vector_literal_expression(float x, float y, float z) : x(x), y(y), z(z) {
    return;
}

constant_value_expression::constant_value_expression(vm::value value) : value(value) {
    return;
}

identifier_expression::identifier_expression(const std::string& identifier) : identifier(identifier) {
    return;
}

subscript_expression::subscript_expression(const std::string& base, expression* index) : base(base), index(index) {
    return;
}

method_call_expression::method_call_expression(const std::string& base, std::vector<expression*>* arguments)
    : base(base), arguments(arguments) {
    return;
}

unary_expression::unary_expression(expression* base, unary_operator op) : base(base), op(op) {
    return;
}

infix_expression::infix_expression(expression* left, expression* right, infix_operator op) : left(left), right(right), op(op) {
    return;
}

assignment_expression::assignment_expression(expression* target, expression* value) : target(target), value(value) {
    return;
}

comma_expression::comma_expression(expression* left, expression* right) : left(left), right(right) {
    return;
}

compound_statement::compound_statement(std::vector<statement*>* code) : code(code) {
    return;
}

expression_statement::expression_statement(expression* expr) : expr(expr) {
    return;
}

call_statement::call_statement(const std::string& label) : label(label) {
    return;
}

if_statement::if_statement(expression* condition, statement* code) : condition(condition), code(code) {
    return;
}

if_else_statement::if_else_statement(expression* condition, statement* code, statement* else_code)
    : condition(condition), code(code), else_code(else_code) {
    return;
}

while_statement::while_statement(expression* condition, statement* code) : condition(condition), code(code) {
    return;
}

do_statement::do_statement(statement* code, expression* condition) : code(code), condition(condition) {
    return;
}

for_expression::for_expression(expression* condition) : condition(condition) {
    return;
}

for_statement::for_statement(for_optional_expression* initializer, for_optional_expression* condition, for_optional_expression* incrementer, statement* code)
    : initializer(initializer), condition(condition), incrementer(incrementer), code(code) {
    return;
}

labeled_statement::labeled_statement(const std::string& label, statement* code) : label(label), code(code) {
    return;
}

translation_unit::translation_unit(unsigned int flags, std::vector<symbol*>* symbols, std::vector<statement*>* code)
    : flags(flags), symbols(symbols), code(code) {
    return;
}

ASTVISITOR_ACCEPT_IMPL(string_fragment_field)
ASTVISITOR_ACCEPT_IMPL(integer_field)
ASTVISITOR_ACCEPT_IMPL(float_field)

ASTVISITOR_ACCEPT_IMPL(bare_extension)
ASTVISITOR_ACCEPT_IMPL(valued_extension)

ASTVISITOR_ACCEPT_IMPL(symbol)
ASTVISITOR_ACCEPT_IMPL(valued_symbol)

ASTVISITOR_ACCEPT_IMPL(constant_value_expression)
ASTVISITOR_ACCEPT_IMPL(string_literal_expression)
ASTVISITOR_ACCEPT_IMPL(integer_literal_expression)
ASTVISITOR_ACCEPT_IMPL(float_literal_expression)
ASTVISITOR_ACCEPT_IMPL(vector_literal_expression)
ASTVISITOR_ACCEPT_IMPL(identifier_expression)
ASTVISITOR_ACCEPT_IMPL(subscript_expression)
ASTVISITOR_ACCEPT_IMPL(method_call_expression)
ASTVISITOR_ACCEPT_IMPL(unary_expression)
ASTVISITOR_ACCEPT_IMPL(infix_expression)
ASTVISITOR_ACCEPT_IMPL(assignment_expression)
ASTVISITOR_ACCEPT_IMPL(comma_expression)

ASTVISITOR_ACCEPT_IMPL(compound_statement)
ASTVISITOR_ACCEPT_IMPL(empty_statement)
ASTVISITOR_ACCEPT_IMPL(expression_statement)
ASTVISITOR_ACCEPT_IMPL(break_statement)
ASTVISITOR_ACCEPT_IMPL(return_statement)
ASTVISITOR_ACCEPT_IMPL(call_statement)
ASTVISITOR_ACCEPT_IMPL(if_statement)
ASTVISITOR_ACCEPT_IMPL(if_else_statement)
ASTVISITOR_ACCEPT_IMPL(while_statement)
ASTVISITOR_ACCEPT_IMPL(do_statement)
ASTVISITOR_ACCEPT_IMPL(for_optional_expression)
ASTVISITOR_ACCEPT_IMPL(for_expression)
ASTVISITOR_ACCEPT_IMPL(for_statement)
ASTVISITOR_ACCEPT_IMPL(labeled_statement)

ASTVISITOR_ACCEPT_IMPL(translation_unit)
