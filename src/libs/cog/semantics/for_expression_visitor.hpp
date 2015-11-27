#pragma once

#include "cog/ast/ast.hpp"
#include "ast/factory.hpp"
#include "cog/script/script.hpp"
#include "cog/script/constant_table.hpp"
#include "cog/script/verb_table.hpp"

namespace gorc {
    namespace cog {

        class for_expression_visitor {
        private:
            script &out_script;
            ast_factory &factory;
            constant_table const &constants;
            verb_table const &verbs;
            bool result_is_used;

        public:
            for_expression_visitor(script &out_script,
                                   ast_factory &factory,
                                   constant_table const &constants,
                                   verb_table const &verbs,
                                   bool result_is_used);

            value_type visit(ast::for_expression &);
            value_type visit(ast::for_empty_expression &);
        };
    }
}
