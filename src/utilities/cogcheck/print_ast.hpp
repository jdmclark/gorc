#pragma once

#include "cog/ast/ast.hpp"
#include "cog/script/script.hpp"

namespace gorc {

    void print_ast(cog::ast::translation_unit &tu,
                   cog::script &script);

}
