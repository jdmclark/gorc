#pragma once

#include "jk/cog/ast/ast.hpp"
#include "jk/cog/script/script.hpp"

namespace gorc {

    void print_ast(cog::ast::translation_unit &tu,
                   cog::script &script);

}
