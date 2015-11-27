#pragma once

#include "cog/ast/ast.hpp"

namespace gorc {
    namespace cog {

        class symbol_extension_field_integer_visitor {
        public:
            std::string const &name;

            symbol_extension_field_integer_visitor(std::string const &name)
                : name(name)
            {
                return;
            }

            int visit(ast_node &) const;
            int visit(ast::integer_field &) const;
        };
    }
}
