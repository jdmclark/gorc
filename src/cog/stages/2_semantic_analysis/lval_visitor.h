#pragma once

#include "cog/ast/visitor.h"
#include "cog/symbols/table.h"
#include "cog/verbs/table.h"

namespace Gorc {
namespace Cog {
namespace Stages {
namespace SemanticAnalysis {

class LValueVisitor : public AST::Visitor {
protected:
	Symbols::SymbolTable& SymbolTable;
	const std::unordered_map<std::string, VM::Value>& ConstantTable;
	Verbs::VerbTable& VerbTable;

	void DefaultAction(const std::string& action, AST::Node& n);

public:
	LValueVisitor(Symbols::SymbolTable& st, const std::unordered_map<std::string, VM::Value>& ct,
		Verbs::VerbTable& vt, Diagnostics::Report& report);

private:
	AVPROTO(IdentifierExpression);
	AVPROTO(SubscriptExpression);
};

}
}
}
}
