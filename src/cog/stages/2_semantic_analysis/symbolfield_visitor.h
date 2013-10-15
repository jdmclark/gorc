#pragma once

#include "cog/ast/visitor.h"
#include <string>

namespace gorc {
namespace cog {
namespace stages {
namespace semantic_analysis {

class symbol_field_visitor : public ast::visitor {
public:
	float float_value;
	bool is_float;

	int int_value;
	bool is_int;

	bool is_numeric;

	std::string str_value;
	bool is_str;

	symbol_field_visitor(diagnostics::report& report);

private:
	ASTCLASS_SYMBOL_FIELD(AVPROTO);
};

}
}
}
}
