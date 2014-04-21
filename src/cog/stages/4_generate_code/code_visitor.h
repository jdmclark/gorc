#pragma once

#include "cog/ast/visitor.h"
#include "cog/ir/printer.h"

namespace gorc {
namespace cog {
namespace stages {
namespace generate_code {

class code_visitor : public ast::visitor {
protected:
    ir::printer& Printer;
    int& nextLabelNumber;

    const std::string BreakLabel;

    std::string generate_label(const std::string& prefix);

public:
    code_visitor(int& nextLabelNumber, ir::printer& printer, diagnostics::report& report, const std::string breakLabel = "");

private:
    ASTCLASS_STATEMENT(AVPROTO)
};

}
}
}
}
