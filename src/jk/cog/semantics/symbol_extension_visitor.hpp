#pragma once

#include "jk/cog/ast/ast.hpp"
#include "jk/cog/script/value.hpp"
#include "jk/cog/script/source_type.hpp"
#include "utility/flag_set.hpp"

namespace gorc {
    namespace cog {

        class symbol_extension_visitor {
        public:
            value_type symbol_type;
            bool local = false;
            maybe<int> linkid;
            bool nolink = false;
            std::string desc;
            maybe<flag_set<source_type>> mask;

            symbol_extension_visitor(value_type type);

            void visit(ast_list_node<ast::symbol_extension*> &);
            void visit(ast::bare_extension &);
            void visit(ast::valued_extension &);
        };

    }
}
