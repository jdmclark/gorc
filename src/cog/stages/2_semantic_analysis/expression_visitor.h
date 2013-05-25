#pragma once

#include "cog/ast/visitor.h"
#include "cog/symbols/table.h"
#include "cog/verbs/table.h"

namespace Gorc {
namespace Cog {
namespace Stages {
namespace SemanticAnalysis {

class ExpressionVisitor : public AST::Visitor
{
protected:
	Symbols::SymbolTable& SymbolTable;
	const std::unordered_map<std::string, VM::Value>& ConstantTable;
	Verbs::VerbTable& VerbTable;

public:
	VM::Type ExpressionType;

	ExpressionVisitor(Symbols::SymbolTable& st, const std::unordered_map<std::string, VM::Value>& ct,
		Verbs::VerbTable& vt, Diagnostics::Report& report);

private:
	ASTCLASS_EXPRESSION(AVPROTO);
};

}
}
}
}
