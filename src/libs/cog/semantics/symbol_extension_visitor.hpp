#pragma once

#include "cog/ast/ast.hpp"
#include "cog/script/value.hpp"

namespace gorc {
    namespace cog {

        class symbol_extension_visitor {
        public:
            value_type symbol_type;
            bool local = false;
            maybe<int> linkid;
            bool nolink = false;
            std::string desc;
            maybe<int> mask;

            symbol_extension_visitor(value_type type);

            void visit(ast_list_node<ast::symbol_extension*> &);
            void visit(ast::bare_extension &);
            void visit(ast::valued_extension &);
        };

    }
}
