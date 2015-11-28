#pragma once

#include "io/path.hpp"
#include "ast.hpp"
#include "ast/factory.hpp"

namespace gorc {

    translation_unit* parse_shell_script(path const &fn, ast_factory &);

}
