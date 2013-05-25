#pragma once

#include "cog/ast/visitor.h"
#include "cog/ir/printer.h"

namespace Gorc {
namespace Cog {
namespace Stages {
namespace GenerateCode {

class NonValuedExpressionVisitor : public AST::Visitor
{
protected:
	IR::Printer& Printer;

public:
	NonValuedExpressionVisitor(IR::Printer& printer, Diagnostics::Report& report);

private:
	ASTCLASS_FINAL_EXPRESSION(AVPROTO);
};

}
}
}
}
