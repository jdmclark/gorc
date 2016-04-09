#pragma once

#include "lexer.hpp"
#include "ast/factory.hpp"
#include "jk/cog/ast/ast.hpp"

namespace gorc {
    namespace cog {

        ast::translation_unit* parse_cog(ast_factory &factory, cog_tokenizer &);

    }
}
