#pragma once

#include "libold/cog/ast/node.hpp"
#include "libold/cog/ir/printer.hpp"
#include <unordered_map>

namespace gorc {
namespace cog {
namespace stages {
namespace generate_code {

class lvalue_expression_visitor {
protected:
    ir::printer& Printer;
    std::unordered_map<std::string, vm::value> const &constant_table;

public:
    lvalue_expression_visitor(ir::printer& printer,
                              std::unordered_map<std::string, vm::value> const &ct);

    void visit(ast::node &);
    void visit(ast::identifier_expression &);
    void visit(ast::subscript_expression &);
};

}
}
}
}
