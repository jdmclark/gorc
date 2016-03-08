#pragma once

#include "libold/cog/ast/node.hpp"
#include "libold/cog/vm/value.hpp"
#include "libold/cog/symbols/type.hpp"

namespace gorc {
namespace cog {
namespace stages {
namespace semantic_analysis {

class symbol_extension_visitor
{
public:
    bool local;

    bool linkid_defined;
    int linkid;

    bool nolink;

    std::string desc;

    bool mask_defined;
    int mask;

    symbol_extension_visitor(symbols::symbol_type type);

    void visit(ast::bare_extension &ext);
    void visit(ast::valued_extension &ext);

private:
    symbols::symbol_type symbol_type;

    bool validate_extension(const std::string& name);
};

}
}
}
}
