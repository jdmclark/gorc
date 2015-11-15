#pragma once

#include <string>
#include "io/input_stream.hpp"
#include "cog/ast/factory.hpp"

namespace gorc {
    namespace cog {

        class grammar {
        private:
            input_stream &file;

        public:
            ast::factory &factory;

            grammar(input_stream &file, ast::factory &factory);

            maybe<ast::translation_unit *> parse();
        };

    }
}
