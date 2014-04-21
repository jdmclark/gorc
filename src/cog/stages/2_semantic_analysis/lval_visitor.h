#pragma once

#include "cog/ast/visitor.h"
#include "cog/symbols/table.h"
#include "cog/verbs/table.h"

namespace gorc {
namespace cog {
namespace stages {
namespace semantic_analysis {

class lvalue_visitor : public ast::visitor {
protected:
    symbols::symbol_table& symbol_table;
    const std::unordered_map<std::string, vm::value>& ConstantTable;
    verbs::verb_table& verb_table;

    void default_action(const std::string& action, ast::node& n);

public:
    lvalue_visitor(symbols::symbol_table& st, const std::unordered_map<std::string, vm::value>& ct,
        verbs::verb_table& vt, diagnostics::report& report);

private:
    AVPROTO(identifier_expression)
    AVPROTO(subscript_expression)
};

}
}
}
}
