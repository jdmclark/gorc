#pragma once

#include "cog/ast/visitor.h"
#include "cog/ir/printer.h"

namespace Gorc {
namespace Cog {
namespace Stages {
namespace GenerateCode {

class CodeVisitor : public AST::Visitor {
protected:
	IR::Printer& Printer;
	int& nextLabelNumber;

	const std::string BreakLabel;

	std::string GenerateLabel(const std::string& prefix);

public:
	CodeVisitor(int& nextLabelNumber, IR::Printer& printer, Diagnostics::Report& report, const std::string breakLabel = "");

private:
	ASTCLASS_STATEMENT(AVPROTO);
};

}
}
}
}
