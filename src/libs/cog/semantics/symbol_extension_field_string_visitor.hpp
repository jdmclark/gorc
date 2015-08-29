#pragma once

#include "cog/ast/ast.hpp"

namespace gorc {
    namespace cog {

        class symbol_extension_field_string_visitor {
        public:
            std::string visit(ast::string_fragment_field &) const;
            std::string visit(ast::integer_field &) const;
            std::string visit(ast::float_field &) const;
        };

    }
}
