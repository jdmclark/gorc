#pragma once

#include <string>
#include "io/input_stream.hpp"
#include "cog/ast/factory.hpp"

namespace gorc {
    namespace cog {

        class grammar {
        private:
            input_stream &file;

            // Scanner initialization members, implemented in lexer.l
            void init_scanner();
            void destroy_scanner();

            void *scanner;

            ast::translation_unit *tu = nullptr;

        public:
            ast::factory &factory;

            grammar(input_stream &file, ast::factory &factory);
            ~grammar();

            char get_next();
            void* get_scanner() const;

            ast::translation_unit* parse();

            std::string token_buffer;

            inline void set_return_value(ast::translation_unit *value)
            {
                tu = value;
            }
        };

    }
}
