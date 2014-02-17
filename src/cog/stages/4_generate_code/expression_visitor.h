#pragma once

#include "cog/ast/visitor.h"
#include "cog/ir/printer.h"

namespace gorc {
namespace cog {
namespace stages {
namespace generate_code {

class expression_visitor : public ast::visitor {
protected:
    ir::printer& Printer;

public:
    expression_visitor(ir::printer& printer, diagnostics::report& report);

private:
    ASTCLASS_FINAL_EXPRESSION(AVPROTO);
};

}
}
}
}
