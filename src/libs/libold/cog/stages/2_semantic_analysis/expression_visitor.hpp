#pragma once

#include "libold/cog/ast/node.hpp"
#include "libold/cog/symbols/table.hpp"
#include "libold/cog/verbs/table.hpp"

namespace gorc {
namespace cog {
namespace stages {
namespace semantic_analysis {

class expression_visitor
{
protected:
    symbols::symbol_table &symbol_table;
    const std::unordered_map<std::string, vm::value> &ConstantTable;
    verbs::verb_table &verb_table;

public:
    vm::type expression_type;

    expression_visitor(symbols::symbol_table &st,
                       std::unordered_map<std::string, vm::value> const &ct,
                       verbs::verb_table &vt);

    void visit(ast::string_literal_expression&);
    void visit(ast::integer_literal_expression&);
    void visit(ast::float_literal_expression&);
    void visit(ast::vector_literal_expression&);
    void visit(ast::identifier_expression&);
    void visit(ast::subscript_expression&);
    void visit(ast::method_call_expression&);
    void visit(ast::unary_expression&);
    void visit(ast::infix_expression&);
    void visit(ast::assignment_expression&);
    void visit(ast::comma_expression&);
    void visit(ast::for_empty_expression&);
    void visit(ast::for_expression&);
};

}
}
}
}
