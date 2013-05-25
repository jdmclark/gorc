#pragma once

#include "cog/ast/visitor.h"
#include "cog/symbols/table.h"

#include <string>
#include <unordered_set>

namespace Gorc {
namespace Cog {
namespace Stages {
namespace SemanticAnalysis {

class LabelVisitor : public AST::Visitor {
protected:
	std::unordered_set<std::string>& SeenLabels;

public:
	LabelVisitor(std::unordered_set<std::string>& SeenLabels, Diagnostics::Report& report);

private:
	ASTCLASS_STATEMENT(AVPROTO);
};

}
}
}
}
