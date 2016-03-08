#pragma once

#include "log/log.hpp"
#include "log/diagnostic_context_location.hpp"
#include "libold/cog/vm/value.hpp"
#include "utility/contains_type.hpp"
#include "utility/maybe.hpp"
#include <typeindex>
#include <string>
#include <vector>
#include <type_traits>

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

class node {
protected:
    node(diagnostic_context_location const &loc);

public:
    virtual ~node();

    diagnostic_context_location location;

    inline operator gorc::diagnostic_context_location() const
    {
        return location;
    }
};

template <typename DerivedT>
class visitable_node : public node {
public:
    using node::node;

    template <typename VisitorT>
    auto accept(VisitorT &visitor)
        -> decltype(visitor.visit(*static_cast<DerivedT*>(this)))
    {
        diagnostic_context dc(*this);
        return visitor.visit(*static_cast<DerivedT*>(this));
    }

    template <typename VisitorT>
    auto accept(VisitorT const &visitor)
        -> decltype(visitor.visit(*static_cast<DerivedT*>(this)))
    {
        diagnostic_context dc(*this);
        return visitor.visit(*static_cast<DerivedT*>(this));
    }
};

template <typename ...AllowedT>
class variant_node : public node {
private:
    std::type_index tag;
    node *value;

    template <typename VisitorT, typename HeadT, typename TailHeadT, typename ...TailT>
    auto accept_check(VisitorT &visitor)
        -> typename std::common_type<decltype(visitor.visit(*static_cast<HeadT*>(value))),
                                     decltype(visitor.visit(*static_cast<TailHeadT*>(value))),
                                     decltype(visitor.visit(*static_cast<TailT*>(value)))...>::type
    {
        if(tag == typeid(HeadT)) {
            return accept_check<VisitorT, HeadT>(visitor);
        }
        else {
            return accept_check<VisitorT, TailHeadT, TailT...>(visitor);
        }
    }

    template <typename VisitorT, typename HeadT>
    auto accept_check(VisitorT &visitor)
        -> decltype(visitor.visit(*static_cast<HeadT*>(value)))
    {
        return static_cast<HeadT*>(value)->accept(visitor);
    }

public:
    template <typename T>
    variant_node(T *value)
        : node(value->location)
        , tag(typeid(T))
        , value(value)
    {
        static_assert(contains_type<T, AllowedT...>::value, "type is not allowed");
        return;
    }

    template <typename VisitorT>
    auto accept(VisitorT &visitor)
        -> decltype(this->accept_check<VisitorT, AllowedT...>(visitor))
    {
        return this->accept_check<VisitorT, AllowedT...>(visitor);
    }
};

/* Symbol section */

class string_fragment_field : public visitable_node<string_fragment_field> {
public:
    std::string value;

    string_fragment_field(diagnostic_context_location const &loc, std::string const &value);
};

class integer_field : public visitable_node<integer_field> {
public:
    int value;

    integer_field(diagnostic_context_location const &loc, int value);
};

class float_field : public visitable_node<float_field> {
public:
    float value;

    float_field(diagnostic_context_location const &loc, float value);
};

using symbol_field = variant_node<string_fragment_field,
                                  integer_field,
                                  float_field>;

class bare_extension : public visitable_node<bare_extension> {
public:
    std::string name;

    bare_extension(diagnostic_context_location const &loc, std::string const &name);
};

class valued_extension : public visitable_node<valued_extension> {
public:
    std::string name;
    symbol_field *value;

    valued_extension(diagnostic_context_location const &loc, std::string const &name, symbol_field *value);
};

using symbol_extension = variant_node<bare_extension,
                                      valued_extension>;

class symbol : public visitable_node<symbol> {
public:
    std::string type;
    std::string name;
    std::vector<symbol_extension*>* extensions;
    maybe<symbol_field*> value;

    symbol(diagnostic_context_location const &loc,
           std::string const &type,
           std::string const &name,
           std::vector<symbol_extension*>* extensions,
           maybe<symbol_field*> value = nothing);
};

/* Expressions */
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
using expression = variant_node<string_literal_expression,
                                integer_literal_expression,
                                float_literal_expression,
                                vector_literal_expression,
                                identifier_expression,
                                subscript_expression,
                                method_call_expression,
                                unary_expression,
                                infix_expression,
                                assignment_expression,
                                comma_expression>;

class string_literal_expression : public visitable_node<string_literal_expression> {
public:
    std::string value;
    char const *stored_string = nullptr;

    string_literal_expression(diagnostic_context_location const &loc, std::string const &value);
};

class integer_literal_expression : public visitable_node<integer_literal_expression> {
public:
    int value;

    integer_literal_expression(diagnostic_context_location const &loc, int value);
};

class float_literal_expression : public visitable_node<float_literal_expression> {
public:
    float value;

    float_literal_expression(diagnostic_context_location const &loc, float value);
};

class vector_literal_expression : public visitable_node<vector_literal_expression> {
public:
    float x, y, z;

    vector_literal_expression(diagnostic_context_location const &loc, float x, float y, float z);
};

class identifier_expression : public visitable_node<identifier_expression> {
public:
    std::string identifier;

    identifier_expression(diagnostic_context_location const &loc, std::string const &identifier);
};

class subscript_expression : public visitable_node<subscript_expression> {
public:
    std::string base;
    expression *index;

    subscript_expression(diagnostic_context_location const &loc, std::string const &base, expression *index);
};

class method_call_expression : public visitable_node<method_call_expression> {
public:
    std::string base;
    std::vector<expression*>* arguments;

    method_call_expression(diagnostic_context_location const &loc,
                           std::string const &base,
                           std::vector<expression*>* arguments);
};

class unary_expression : public visitable_node<unary_expression> {
public:
    expression *base;
    unary_operator op;

    unary_expression(diagnostic_context_location const &loc,
                     expression *base,
                     unary_operator op);
};

class infix_expression : public visitable_node<infix_expression> {
public:
    expression *left;
    expression *right;
    infix_operator op;

    infix_expression(diagnostic_context_location const &loc,
                     expression *left,
                     expression *right,
                     infix_operator op);
};

class assignment_expression : public visitable_node<assignment_expression> {
public:
    expression *target;
    expression *value;

    assignment_expression(diagnostic_context_location const &loc,
                          expression *target,
                          expression *value);
};

class comma_expression : public visitable_node<comma_expression> {
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
using statement = variant_node<compound_statement,
                               empty_statement,
                               expression_statement,
                               break_statement,
                               return_statement,
                               call_statement,
                               if_statement,
                               if_else_statement,
                               while_statement,
                               do_statement,
                               for_statement,
                               labeled_statement>;

class compound_statement : public visitable_node<compound_statement> {
public:
    std::vector<statement*>* code;

    compound_statement(diagnostic_context_location const &loc, std::vector<statement*>* code);
};

class empty_statement : public visitable_node<empty_statement> {
public:
    empty_statement(diagnostic_context_location const &loc);
};

class expression_statement : public visitable_node<expression_statement> {
public:
    expression *expr;

    expression_statement(diagnostic_context_location const &loc, expression *expr);
};

class break_statement : public visitable_node<break_statement> {
public:
    break_statement(diagnostic_context_location const &loc);
};

class return_statement : public visitable_node<return_statement> {
public:
    return_statement(diagnostic_context_location const &loc);
};

class call_statement : public visitable_node<call_statement> {
public:
    std::string label;

    call_statement(diagnostic_context_location const &loc, std::string const &label);
};

class if_statement : public visitable_node<if_statement> {
public:
    expression *condition;
    statement *code;

    if_statement(diagnostic_context_location const &loc, expression *condition, statement *code);
};

class if_else_statement : public visitable_node<if_else_statement> {
public:
    expression *condition;
    statement *code;
    statement *else_code;

    if_else_statement(diagnostic_context_location const &loc,
                      expression *condition,
                      statement *code,
                      statement *else_code);
};

class while_statement : public visitable_node<while_statement> {
public:
    expression *condition;
    statement *code;

    while_statement(diagnostic_context_location const &loc, expression *condition, statement *code);
};

class do_statement : public visitable_node<do_statement> {
public:
    statement *code;
    expression *condition;

    do_statement(diagnostic_context_location const &loc, statement *code, expression *condition);
};

class for_empty_expression : public visitable_node<for_empty_expression> {
public:
    for_empty_expression(diagnostic_context_location const &loc);
};

class for_expression : public visitable_node<for_expression> {
public:
    expression *condition;

    for_expression(diagnostic_context_location const &loc, expression *condition);
};

using for_optional_expression = variant_node<for_empty_expression,
                                             for_expression>;

class for_statement : public visitable_node<for_statement> {
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

class labeled_statement : public visitable_node<labeled_statement> {
public:
    std::string label;
    statement *code;

    labeled_statement(diagnostic_context_location const &loc, std::string const &label, statement *code);
};

/* Translation unit */
class translation_unit : public visitable_node<translation_unit> {
public:
    unsigned int flags;
    std::vector<symbol*>* symbols;
    std::vector<statement*>* code;

    translation_unit(diagnostic_context_location const &loc,
                     unsigned int flags,
                     std::vector<symbol*>* symbols,
                     std::vector<statement*>* code);
};

}
}
}
