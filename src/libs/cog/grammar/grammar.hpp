#pragma once

#include <string>
#include "io/input_stream.hpp"
#include "ast/factory.hpp"
#include "cog/ast/ast.hpp"

namespace gorc {
    namespace cog {

        class grammar {
        private:
            input_stream &file;

        public:
            ast_factory &factory;

            grammar(input_stream &file, ast_factory &factory);

            maybe<ast::translation_unit *> parse();
        };

    }
}
