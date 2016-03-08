#pragma once

#include "libold/cog/ast/node.hpp"
#include "libold/cog/symbols/table.hpp"
#include "libold/cog/verbs/table.hpp"

#include <unordered_set>
#include <string>

namespace gorc {
namespace cog {
namespace stages {
namespace semantic_analysis {

class code_visitor {
protected:
    symbols::symbol_table& symbol_table;
    const std::unordered_map<std::string, vm::value>& ConstantTable;
    verbs::verb_table& verb_table;
    const std::unordered_set<std::string>& SeenLabels;
    const bool inside_loop;
    const bool inside_block;

public:
    code_visitor(symbols::symbol_table& st,
        const std::unordered_map<std::string, vm::value>& constantTable,
        verbs::verb_table& vt, const std::unordered_set<std::string>& SeenLabels,
        bool inside_loop = false, bool inside_block = false);

    void visit(ast::compound_statement &s);
    void visit(ast::empty_statement &s);
    void visit(ast::expression_statement &s);
    void visit(ast::break_statement &s);
    void visit(ast::return_statement &s);
    void visit(ast::call_statement &s);
    void visit(ast::if_statement &s);
    void visit(ast::if_else_statement &s);
    void visit(ast::while_statement &s);
    void visit(ast::do_statement &s);
    void visit(ast::for_statement &s);
    void visit(ast::labeled_statement &s);
};

}
}
}
}
