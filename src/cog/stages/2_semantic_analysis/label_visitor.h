#pragma once

#include "cog/ast/visitor.h"
#include "cog/symbols/table.h"

#include <string>
#include <unordered_set>

namespace gorc {
namespace cog {
namespace stages {
namespace semantic_analysis {

class label_visitor : public ast::visitor {
protected:
    std::unordered_set<std::string>& SeenLabels;

public:
    label_visitor(std::unordered_set<std::string>& SeenLabels, diagnostics::report& report);

private:
    ASTCLASS_STATEMENT(AVPROTO)
};

}
}
}
}
