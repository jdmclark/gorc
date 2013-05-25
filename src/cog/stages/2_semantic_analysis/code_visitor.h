#pragma once

#include "cog/ast/visitor.h"
#include "cog/symbols/table.h"
#include "cog/verbs/table.h"

#include <unordered_set>
#include <string>

namespace Gorc {
namespace Cog {
namespace Stages {
namespace SemanticAnalysis {

class CodeVisitor : public AST::Visitor {
protected:
	Symbols::SymbolTable& SymbolTable;
	const std::unordered_map<std::string, VM::Value>& ConstantTable;
	Verbs::VerbTable& VerbTable;
	const std::unordered_set<std::string>& SeenLabels;
	const bool inside_loop;
	const bool inside_block;

public:
	CodeVisitor(Symbols::SymbolTable& st,
		const std::unordered_map<std::string, VM::Value>& constantTable,
		Verbs::VerbTable& vt, const std::unordered_set<std::string>& SeenLabels,
		Diagnostics::Report& report, bool inside_loop = false, bool inside_block = false);

private:
	ASTCLASS_STATEMENT(AVPROTO);
};

}
}
}
}
