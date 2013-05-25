#pragma once

#include "cog/ast/visitor.h"
#include <string>

namespace Gorc {
namespace Cog {
namespace Stages {
namespace SemanticAnalysis {

class SymbolFieldVisitor : public AST::Visitor {
public:
	float float_value;
	bool is_float;

	int int_value;
	bool is_int;

	bool is_numeric;

	std::string str_value;
	bool is_str;

	SymbolFieldVisitor(Diagnostics::Report& report);

private:
	ASTCLASS_SYMBOL_FIELD(AVPROTO);
};

}
}
}
}
