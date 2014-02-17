#pragma once

#include "cog/ast/visitor.h"
#include "cog/symbols/table.h"
#include "cog/symbols/type.h"

#include <unordered_set>
#include <string>

namespace gorc {
namespace cog {
namespace stages {
namespace semantic_analysis {

class symbol_visitor : public ast::visitor
{
protected:
    symbols::symbol_table& symbol_table;
    const std::unordered_set<std::string>& SeenLabels;

public:
    symbol_visitor(symbols::symbol_table& st, const std::unordered_set<std::string>& SeenLabels, diagnostics::report& report);

private:
    symbols::symbol_type get_type(ast::symbol& symbol);

    ASTCLASS_SYMBOL(AVPROTO);
};

}
}
}
}
