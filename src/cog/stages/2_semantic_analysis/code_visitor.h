#pragma once

#include "cog/ast/visitor.h"
#include "cog/symbols/table.h"
#include "cog/verbs/table.h"

#include <unordered_set>
#include <string>

namespace gorc {
namespace cog {
namespace stages {
namespace semantic_analysis {

class code_visitor : public ast::visitor {
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
        diagnostics::report& report, bool inside_loop = false, bool inside_block = false);

private:
    ASTCLASS_STATEMENT(AVPROTO)
};

}
}
}
}
