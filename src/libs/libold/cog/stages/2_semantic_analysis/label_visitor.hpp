#pragma once

#include "libold/cog/ast/node.hpp"
#include "libold/cog/symbols/table.hpp"

#include <string>
#include <unordered_set>

namespace gorc {
namespace cog {
namespace stages {
namespace semantic_analysis {

class label_visitor {
protected:
    std::unordered_set<std::string>& SeenLabels;

public:
    label_visitor(std::unordered_set<std::string>& SeenLabels);

    void visit(ast::compound_statement&);
    void visit(ast::empty_statement&);
    void visit(ast::expression_statement&);
    void visit(ast::break_statement&);
    void visit(ast::return_statement&);
    void visit(ast::call_statement&);
    void visit(ast::if_statement&);
    void visit(ast::if_else_statement&);
    void visit(ast::while_statement&);
    void visit(ast::do_statement&);
    void visit(ast::for_statement&);
    void visit(ast::labeled_statement&);
};

}
}
}
}
