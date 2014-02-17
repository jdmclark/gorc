#pragma once

#include "cog/ast/visitor.h"
#include "cog/vm/value.h"
#include "cog/ast/factory.h"
#include "cog/symbols/table.h"

#include <string>
#include <unordered_map>

namespace gorc {
namespace cog {
namespace stages {
namespace constant_folding {

class code_visitor : public ast::visitor
{
protected:
    ast::factory& Factory;
    symbols::symbol_table& symbol_table;
    const std::unordered_map<std::string, vm::value>& ConstantTable;

public:
    code_visitor(ast::factory& factory, symbols::symbol_table& symbolTable,
        const std::unordered_map<std::string, vm::value>& constantTable,
        diagnostics::report& report);

private:
    ASTCLASS_STATEMENT(AVPROTO);
};

}
}
}
}
