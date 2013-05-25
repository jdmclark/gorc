#pragma once

#include "cog/ast/visitor.h"
#include "cog/ir/printer.h"

namespace Gorc {
namespace Cog {
namespace Stages {
namespace GenerateCode {

class LValueExpressionVisitor : public AST::Visitor {
protected:
	IR::Printer& Printer;

public:
	LValueExpressionVisitor(IR::Printer& printer, Diagnostics::Report& report);

private:
	AVPROTO(IdentifierExpression);
	AVPROTO(SubscriptExpression);
};

}
}
}
}
