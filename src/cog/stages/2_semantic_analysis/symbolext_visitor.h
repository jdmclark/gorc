#pragma once

#include "cog/ast/visitor.h"
#include "cog/vm/value.h"
#include "cog/symbols/type.h"

namespace Gorc {
namespace Cog {
namespace Stages {
namespace SemanticAnalysis {

class SymbolExtensionVisitor : public AST::Visitor
{
public:
	bool local;

	bool linkid_defined;
	int linkid;

	bool nolink;

	std::string desc;

	bool mask_defined;
	int mask;

	SymbolExtensionVisitor(Symbols::SymbolType type, Diagnostics::Report& report);

private:
	Symbols::SymbolType SymbolType;

	ASTCLASS_SYMBOL_EXTENSION(AVPROTO);

	bool ValidateExtension(const std::string& name, Diagnostics::ErrorLocation& location);
};

}
}
}
}
