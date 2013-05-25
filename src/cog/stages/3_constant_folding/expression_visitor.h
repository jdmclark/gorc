#pragma once

#include "cog/ast/visitor.h"
#include "cog/vm/value.h"
#include "cog/ast/factory.h"
#include "cog/symbols/table.h"

#include <string>
#include <unordered_map>

namespace Gorc {
namespace Cog {
namespace Stages {
namespace ConstantFolding {

class ExpressionVisitor : public AST::Visitor {
protected:
	AST::Factory& Factory;
	Symbols::SymbolTable& SymbolTable;
	const std::unordered_map<std::string, VM::Value>& ConstantTable;

private:
	bool isConstant;
	VM::Value constantValue;
	AST::Expression* visitedExpression;

public:
	ExpressionVisitor(AST::Factory& factory, Symbols::SymbolTable& symbolTable,
		const std::unordered_map<std::string, VM::Value>& constantTable,
		Diagnostics::Report& report);

	inline AST::Expression* GetSubstitution() const {
		if(isConstant) {
			return Factory.MakeConstantValueExpression(constantValue, visitedExpression->Location);
		}
		else {
			return visitedExpression;
		}
	}

	inline VM::Value GetConstantValue() const {
		return constantValue;
	}

	inline bool IsConstant() const {
		return isConstant;
	}

private:
	ASTCLASS_EXPRESSION(AVPROTO);
};

}
}
}
}
