#pragma once

#include "libold/cog/ast/node.hpp"
#include "libold/cog/symbols/table.hpp"
#include "libold/cog/verbs/table.hpp"

namespace gorc {
namespace cog {
namespace stages {
namespace semantic_analysis {

class lvalue_visitor {
protected:
    symbols::symbol_table& symbol_table;
    const std::unordered_map<std::string, vm::value>& ConstantTable;
    verbs::verb_table& verb_table;

    void default_action(const std::string& action, ast::node& n);

public:
    lvalue_visitor(symbols::symbol_table& st, const std::unordered_map<std::string, vm::value>& ct,
        verbs::verb_table& vt);

    void visit(ast::node &);
    void visit(ast::identifier_expression &);
    void visit(ast::subscript_expression &);
};

}
}
}
}
