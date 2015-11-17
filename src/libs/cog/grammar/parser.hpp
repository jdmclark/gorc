#pragma once

#include "lexer.hpp"
#include "cog/ast/factory.hpp"

namespace gorc {
    namespace cog {

        ast::translation_unit* parse_cog(ast::factory &factory, cog_tokenizer &);

    }
}
