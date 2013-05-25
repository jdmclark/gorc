#pragma once

#include "cog/ast/visitor.h"
#include "cog/symbols/table.h"
#include "cog/symbols/type.h"

#include <unordered_set>
#include <string>

namespace Gorc {
namespace Cog {
namespace Stages {
namespace SemanticAnalysis {

class SymbolVisitor : public AST::Visitor
{
protected:
	Symbols::SymbolTable& SymbolTable;
	const std::unordered_set<std::string>& SeenLabels;

public:
	SymbolVisitor(Symbols::SymbolTable& st, const std::unordered_set<std::string>& SeenLabels, Diagnostics::Report& report);

private:
	Symbols::SymbolType GetType(AST::Symbol& symbol);

	ASTCLASS_SYMBOL(AVPROTO);
};

}
}
}
}
