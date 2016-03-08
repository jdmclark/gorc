#pragma once

#include "libold/cog/ast/node.hpp"
#include "libold/cog/symbols/table.hpp"
#include "libold/cog/symbols/type.hpp"

#include <unordered_set>
#include <string>

namespace gorc {
namespace cog {
namespace stages {
namespace semantic_analysis {

class symbol_visitor
{
protected:
    symbols::symbol_table& symbol_table;
    const std::unordered_set<std::string>& SeenLabels;

public:
    symbol_visitor(symbols::symbol_table& st, const std::unordered_set<std::string>& SeenLabels);

    void visit(ast::symbol &);

private:
    symbols::symbol_type get_type(ast::symbol& symbol);
    vm::value get_value(ast::symbol& symbol);
};

}
}
}
}
