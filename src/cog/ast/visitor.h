#pragma once

#include "node.h"
#include "base/diagnostics/report.h"

#define AVPROTO(x) virtual void visit_##x(ast::x&);
#define AVCONSUME(x) virtual void visit_##x(ast::x&) { }
#define AVDEFAULTACTION(x) void gorc::cog::ast::visitor::visit_##x(x& e) { default_action(#x, e); }

#define ASTCLASS_SYMBOL_FIELD(x) \
    x(string_fragment_field) \
    x(integer_field) \
    x(float_field)

#define ASTCLASS_SYMBOL_EXTENSION(x) \
    x(bare_extension) \
    x(valued_extension)

#define ASTCLASS_SYMBOL(x) \
    x(symbol) \
    x(valued_symbol)

#define ASTCLASS_EXPRESSION(x) \
    x(string_literal_expression) \
    x(integer_literal_expression) \
    x(float_literal_expression) \
    x(vector_literal_expression) \
    x(identifier_expression) \
    x(subscript_expression) \
    x(method_call_expression) \
    x(unary_expression) \
    x(infix_expression) \
    x(assignment_expression) \
    x(comma_expression) \
    x(for_optional_expression) \
    x(for_expression)

#define ASTCLASS_FINAL_EXPRESSION(x) \
    x(constant_value_expression) \
    x(identifier_expression) \
    x(subscript_expression) \
    x(method_call_expression) \
    x(unary_expression) \
    x(infix_expression) \
    x(assignment_expression) \
    x(comma_expression) \
    x(for_optional_expression) \
    x(for_expression)

#define ASTCLASS_STATEMENT(x) \
    x(compound_statement) \
    x(empty_statement) \
    x(expression_statement) \
    x(break_statement) \
    x(return_statement) \
    x(call_statement) \
    x(if_statement) \
    x(if_else_statement) \
    x(while_statement) \
    x(do_statement) \
    x(for_statement) \
    x(labeled_statement)

#define ASTCLASS_TRANSLATION_UNIT(x) \
    x(translation_unit)

#define ASTCLASS_ALL(x) \
    ASTCLASS_SYMBOL_FIELD(x) \
    ASTCLASS_SYMBOL_EXTENSION(x) \
    ASTCLASS_SYMBOL(x) \
    ASTCLASS_EXPRESSION(x) \
    ASTCLASS_STATEMENT(x) \
    ASTCLASS_TRANSLATION_UNIT(x) \
    x(constant_value_expression)

namespace gorc {
namespace cog {
namespace ast {

class visitor
{
protected:
    const std::string visitor_name;
    diagnostics::report& report;

    virtual void default_action(const std::string& action, node& n);

    visitor(const std::string& name, diagnostics::report& report);

public:
    virtual ~visitor();

    ASTCLASS_ALL(AVPROTO)
};

}
}
}
