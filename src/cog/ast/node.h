#pragma once

#include "framework/text/location.h"
#include "cog/vm/value.h"
#include <string>
#include <vector>

#define ASTVISITOR_ACCEPT_ABSTRACT                            \
    public:                                                    \
    virtual void accept(gorc::cog::ast::visitor& v) = 0;    \
    private:

#define ASTVISITOR_ACCEPT                                    \
    public:                                                    \
    virtual void accept(gorc::cog::ast::visitor& v);        \
    private:

#define ASTVISITOR_ACCEPT_IMPL( x )                            \
    void gorc::cog::ast::x::accept(visitor& v) {            \
        v.visit_##x(*this);                                    \
        return;                                                \
    }

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

class visitor;

class node {
    ASTVISITOR_ACCEPT_ABSTRACT
protected:
    node();

public:
    virtual ~node();

    text::location location;
};

/* Symbol section */

class symbol_field : public node {
    ASTVISITOR_ACCEPT_ABSTRACT
public:
};

class string_fragment_field : public symbol_field {
    ASTVISITOR_ACCEPT
public:
    std::string value;

    string_fragment_field(const std::string& value);
};

class integer_field : public symbol_field {
    ASTVISITOR_ACCEPT
public:
    int value;

    integer_field(int value);
};

class float_field : public symbol_field {
    ASTVISITOR_ACCEPT
public:
    float value;

    float_field(float value);
};

class symbol_extension : public node {
    ASTVISITOR_ACCEPT_ABSTRACT
public:
};

class bare_extension : public symbol_extension {
    ASTVISITOR_ACCEPT
public:
    std::string name;

    bare_extension(const std::string& name);
};

class valued_extension : public bare_extension {
    ASTVISITOR_ACCEPT
public:
    symbol_field* value;

    valued_extension(const std::string& name, symbol_field* value);
};

class symbol : public node {
    ASTVISITOR_ACCEPT
public:
    std::string type;
    std::string name;
    std::vector<symbol_extension*>* extensions;

    symbol(const std::string& type, const std::string& name, std::vector<symbol_extension*>* extensions);
};

class valued_symbol : public symbol {
    ASTVISITOR_ACCEPT
public:
    symbol_field* value;

    valued_symbol(const std::string& type, const std::string& name, std::vector<symbol_extension*>* extensions, symbol_field* value);
};

/* Expressions */
class expression : public node {
    ASTVISITOR_ACCEPT_ABSTRACT
};

class string_literal_expression : public expression {
    ASTVISITOR_ACCEPT
public:
    std::string value;

    string_literal_expression(const std::string& value);
};

class integer_literal_expression : public expression {
    ASTVISITOR_ACCEPT
public:
    int value;

    integer_literal_expression(int value);
};

class float_literal_expression : public expression {
    ASTVISITOR_ACCEPT
public:
    float value;

    float_literal_expression(float value);
};

class vector_literal_expression : public expression {
    ASTVISITOR_ACCEPT
public:
    float x, y, z;

    vector_literal_expression(float x, float y, float z);
};

class constant_value_expression : public expression {
    ASTVISITOR_ACCEPT
public:
    vm::value value;

    constant_value_expression(vm::value value);
};

class identifier_expression : public expression {
    ASTVISITOR_ACCEPT
public:
    std::string identifier;

    identifier_expression(const std::string& identifier);
};

class subscript_expression : public expression {
    ASTVISITOR_ACCEPT
public:
    std::string base;
    expression* index;

    subscript_expression(const std::string& base, expression* index);
};

class method_call_expression : public expression {
    ASTVISITOR_ACCEPT
public:
    std::string base;
    std::vector<expression*>* arguments;

    method_call_expression(const std::string& base, std::vector<expression*>* arguments);
};

class unary_expression : public expression {
    ASTVISITOR_ACCEPT
public:
    expression* base;
    unary_operator op;

    unary_expression(expression* base, unary_operator op);
};

class infix_expression : public expression {
    ASTVISITOR_ACCEPT
public:
    expression* left;
    expression* right;
    infix_operator op;

    infix_expression(expression* left, expression* right, infix_operator op);
};

class assignment_expression : public expression {
    ASTVISITOR_ACCEPT
public:
    expression* target;
    expression* value;

    assignment_expression(expression* target, expression* value);
};

class comma_expression : public expression {
    ASTVISITOR_ACCEPT
public:
    expression* left;
    expression* right;

    comma_expression(expression* left, expression* right);
};

/* Statements */
class statement : public node {
    ASTVISITOR_ACCEPT_ABSTRACT
};

class compound_statement : public statement {
    ASTVISITOR_ACCEPT
public:
    std::vector<statement*>* code;

    compound_statement(std::vector<statement*>* code);
};

class empty_statement : public statement {
    ASTVISITOR_ACCEPT
};

class expression_statement : public statement {
    ASTVISITOR_ACCEPT
public:
    expression* expr;

    expression_statement(expression* expr);
};

class break_statement : public statement {
    ASTVISITOR_ACCEPT
};

class return_statement : public statement {
    ASTVISITOR_ACCEPT
};

class call_statement : public statement {
    ASTVISITOR_ACCEPT
public:
    std::string label;

    call_statement(const std::string& label);
};

class if_statement : public statement {
    ASTVISITOR_ACCEPT
public:
    expression* condition;
    statement* code;

    if_statement(expression* condition, statement* code);
};

class if_else_statement : public statement {
    ASTVISITOR_ACCEPT
public:
    expression* condition;
    statement* code;
    statement* else_code;

    if_else_statement(expression* condition, statement* code, statement* else_code);
};

class while_statement : public statement {
    ASTVISITOR_ACCEPT
public:
    expression* condition;
    statement* code;

    while_statement(expression* condition, statement* code);
};

class do_statement : public statement {
    ASTVISITOR_ACCEPT
public:
    statement* code;
    expression* condition;

    do_statement(statement* code, expression* condition);
};

class for_optional_expression : public node {
    ASTVISITOR_ACCEPT
};

class for_expression : public for_optional_expression {
    ASTVISITOR_ACCEPT
public:
    expression* condition;

    for_expression(expression* condition);
};

class for_statement : public statement {
    ASTVISITOR_ACCEPT
public:
    for_optional_expression* initializer;
    for_optional_expression* condition;
    for_optional_expression* incrementer;
    statement* code;

    for_statement(for_optional_expression* initializer, for_optional_expression* condition, for_optional_expression* incrementer, statement* code);
};

class labeled_statement : public statement {
    ASTVISITOR_ACCEPT
public:
    std::string label;
    statement* code;

    labeled_statement(const std::string& label, statement* code);
};

/* Translation unit */
class translation_unit : public node {
    ASTVISITOR_ACCEPT
public:
    unsigned int flags;
    std::vector<symbol*>* symbols;
    std::vector<statement*>* code;

    translation_unit(unsigned int flags, std::vector<symbol*>* symbols, std::vector<statement*>* code);
};

}
}
}
