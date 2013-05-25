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

class CodeVisitor : public AST::Visitor
{
protected:
	AST::Factory& Factory;
	Symbols::SymbolTable& SymbolTable;
	const std::unordered_map<std::string, VM::Value>& ConstantTable;

public:
	CodeVisitor(AST::Factory& factory, Symbols::SymbolTable& symbolTable,
		const std::unordered_map<std::string, VM::Value>& constantTable,
		Diagnostics::Report& report);

private:
	ASTCLASS_STATEMENT(AVPROTO);
};

}
}
}
}
