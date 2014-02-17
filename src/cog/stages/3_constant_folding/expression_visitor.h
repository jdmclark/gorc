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

class expression_visitor : public ast::visitor {
protected:
    ast::factory& Factory;
    symbols::symbol_table& symbol_table;
    const std::unordered_map<std::string, vm::value>& ConstantTable;

private:
    bool isConstant;
    vm::value constantvalue;
    ast::expression* visitedExpression;

public:
    expression_visitor(ast::factory& factory, symbols::symbol_table& symbolTable,
        const std::unordered_map<std::string, vm::value>& constantTable,
        diagnostics::report& report);

    inline ast::expression* get_substitution() const {
        if(isConstant) {
            return Factory.make<ast::constant_value_expression>(visitedExpression->location, constantvalue);
        }
        else {
            return visitedExpression;
        }
    }

    inline vm::value get_constant_value() const {
        return constantvalue;
    }

    inline bool is_constant() const {
        return isConstant;
    }

private:
    ASTCLASS_EXPRESSION(AVPROTO);
};

}
}
}
}
