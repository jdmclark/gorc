#pragma once

#include "cog/ast/visitor.h"
#include "cog/symbols/table.h"
#include "cog/verbs/table.h"

namespace gorc {
namespace cog {
namespace stages {
namespace semantic_analysis {

class expression_visitor : public ast::visitor
{
protected:
	symbols::symbol_table& symbol_table;
	const std::unordered_map<std::string, vm::value>& ConstantTable;
	verbs::verb_table& verb_table;

public:
	vm::type expression_type;

	expression_visitor(symbols::symbol_table& st, const std::unordered_map<std::string, vm::value>& ct,
		verbs::verb_table& vt, diagnostics::report& report);

private:
	ASTCLASS_EXPRESSION(AVPROTO);
};

}
}
}
}
