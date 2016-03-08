#pragma once

#include "libold/cog/ast/node.hpp"
#include "libold/cog/ir/printer.hpp"
#include <unordered_map>

namespace gorc {
namespace cog {
namespace stages {
namespace generate_code {

class expression_visitor {
protected:
    ir::printer& Printer;
    std::unordered_map<std::string, vm::value> const &constant_table;

public:
    expression_visitor(ir::printer& printer,
                       std::unordered_map<std::string, vm::value> const &constant_table);

    void visit(ast::string_literal_expression &);
    void visit(ast::integer_literal_expression &);
    void visit(ast::float_literal_expression &);
    void visit(ast::vector_literal_expression &);
    void visit(ast::identifier_expression &);
    void visit(ast::subscript_expression &);
    void visit(ast::method_call_expression &);
    void visit(ast::unary_expression &);
    void visit(ast::infix_expression &);
    void visit(ast::assignment_expression &);
    void visit(ast::comma_expression &);
    void visit(ast::for_empty_expression &);
    void visit(ast::for_expression &);
};

}
}
}
}
