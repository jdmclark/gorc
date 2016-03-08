#include "symbolfield_visitor.hpp"

using namespace gorc::cog::ast;
using gorc::cog::stages::semantic_analysis::symbol_field_visitor;

symbol_field_visitor::symbol_field_visitor()
    : float_value(0.0f), is_float(false), int_value(0), is_int(false), is_numeric(false), is_str(false) {
    return;
}

void symbol_field_visitor::visit(string_fragment_field& v) {
    is_str = true;
    str_value = v.value;
}

void symbol_field_visitor::visit(integer_field& v) {
    is_int = true;
    is_numeric = true;
    int_value = v.value;
    float_value = static_cast<float>(v.value);
}

void symbol_field_visitor::visit(float_field& v) {
    is_float = true;
    is_numeric = true;
    float_value = v.value;
    int_value = static_cast<int>(v.value);
}

