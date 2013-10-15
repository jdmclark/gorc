#pragma once

#include "cog/ast/visitor.h"
#include "cog/ir/printer.h"

namespace gorc {
namespace cog {
namespace stages {
namespace generate_code {

class lvalue_expression_visitor : public ast::visitor {
protected:
	ir::printer& Printer;

public:
	lvalue_expression_visitor(ir::printer& printer, diagnostics::report& report);

private:
	AVPROTO(identifier_expression);
	AVPROTO(subscript_expression);
};

}
}
}
}
