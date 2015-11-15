#pragma once

#include <string>
#include "io/input_stream.hpp"
#include "cog/ast/factory.hpp"
#include "lexer.hpp"

namespace gorc {
    namespace cog {

        class grammar {
        private:
            input_stream &file;
            maybe<ast::translation_unit *> tu;

        public:
            ast::factory &factory;
            cog_tokenizer scanner;

            grammar(input_stream &file, ast::factory &factory);

            maybe<ast::translation_unit *> parse();
        };

    }
}
