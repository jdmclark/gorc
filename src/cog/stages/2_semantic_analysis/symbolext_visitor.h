#pragma once

#include "cog/ast/visitor.h"
#include "cog/vm/value.h"
#include "cog/symbols/type.h"

namespace gorc {
namespace cog {
namespace stages {
namespace semantic_analysis {

class symbol_extension_visitor : public ast::visitor
{
public:
    bool local;

    bool linkid_defined;
    int linkid;

    bool nolink;

    std::string desc;

    bool mask_defined;
    int mask;

    symbol_extension_visitor(symbols::symbol_type type, diagnostics::report& report);

private:
    symbols::symbol_type symbol_type;

    ASTCLASS_SYMBOL_EXTENSION(AVPROTO);

    bool validate_extension(const std::string& name, text::location& location);
};

}
}
}
}
