#pragma once

#include "cog/ast/ast.hpp"
#include "cog/ast/factory.hpp"
#include "cog/script/script.hpp"
#include "cog/script/constant_table.hpp"
#include "cog/script/verb_table.hpp"

namespace gorc {
    namespace cog {

        class for_expression_visitor {
        private:
            script &out_script;
            ast::factory &factory;
            constant_table const &constants;
            verb_table const &verbs;

        public:
            for_expression_visitor(script &out_script,
                                   ast::factory &factory,
                                   constant_table const &constants,
                                   verb_table const &verbs);

            value_type visit(ast::for_expression &);
            value_type visit(ast::for_empty_expression &);
        };
    }
}
