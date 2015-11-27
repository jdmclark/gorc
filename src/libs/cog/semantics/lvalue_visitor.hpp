#pragma once

#include "cog/ast/ast.hpp"
#include "ast/factory.hpp"
#include "cog/script/script.hpp"
#include "cog/script/constant_table.hpp"
#include "cog/script/verb_table.hpp"
#include "expr_val.hpp"

namespace gorc {
    namespace cog {

        class lvalue_visitor {
        private:
            script &out_script;
            ast_factory &factory;
            constant_table const &constants;
            verb_table const &verbs;

        public:
            lvalue_visitor(script &out_script,
                           ast_factory &factory,
                           constant_table const &constants,
                           verb_table const &verbs);

            value_type visit(ast_node &);
            value_type visit(ast::identifier_expression &);
            value_type visit(ast::subscript_expression &);
        };

    }
}
