#pragma once

#include "lexer.hpp"
#include "ast/factory.hpp"
#include "ast.hpp"

namespace gorc {

    translation_unit* parse_shell_script(ast_factory &factory, shell_tokenizer &);

}
