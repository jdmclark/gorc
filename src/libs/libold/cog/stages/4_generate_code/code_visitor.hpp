#pragma once

#include "libold/cog/ast/node.hpp"
#include "libold/cog/ir/printer.hpp"
#include <unordered_map>

namespace gorc {
namespace cog {
namespace stages {
namespace generate_code {

class code_visitor {
protected:
    ir::printer& Printer;
    std::unordered_map<std::string, vm::value> const &constant_table;
    int& nextLabelNumber;

    const std::string BreakLabel;

    std::string generate_label(const std::string& prefix);

public:
    code_visitor(int& nextLabelNumber,
                 ir::printer& printer,
                 std::unordered_map<std::string, vm::value> const &constant_table,
                 const std::string breakLabel = "");

    void visit(ast::compound_statement &);
    void visit(ast::empty_statement &);
    void visit(ast::expression_statement &);
    void visit(ast::break_statement &);
    void visit(ast::return_statement &);
    void visit(ast::call_statement &);
    void visit(ast::if_statement &);
    void visit(ast::if_else_statement &);
    void visit(ast::while_statement &);
    void visit(ast::do_statement &);
    void visit(ast::for_statement &);
    void visit(ast::labeled_statement &);
};

}
}
}
}
