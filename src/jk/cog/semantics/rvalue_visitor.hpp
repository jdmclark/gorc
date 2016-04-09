#pragma once

#include "jk/cog/ast/ast.hpp"
#include "ast/factory.hpp"
#include "jk/cog/script/script.hpp"
#include "jk/cog/script/constant_table.hpp"
#include "jk/cog/script/verb_table.hpp"
#include "expr_val.hpp"

namespace gorc {
    namespace cog {

        class rvalue_visitor {
        private:
            script &out_script;
            ast_factory &factory;
            constant_table const &constants;
            verb_table const &verbs;
            bool result_is_used;

        public:
            rvalue_visitor(script &out_script,
                           ast_factory &factory,
                           constant_table const &constants,
                           verb_table const &verbs,
                           bool result_is_used);

            expr_val visit(ast::immediate_expression &);
            expr_val visit(ast::string_literal_expression &);
            expr_val visit(ast::integer_literal_expression &);
            expr_val visit(ast::float_literal_expression &);
            expr_val visit(ast::vector_literal_expression &);
            expr_val visit(ast::identifier_expression &);
            expr_val visit(ast::subscript_expression &);
            expr_val visit(ast::method_call_expression &);
            expr_val visit(ast::unary_expression &);
            expr_val visit(ast::infix_expression &);
            expr_val visit(ast::assignment_expression &);
            expr_val visit(ast::comma_expression &);
        };

        void visit_and_fold_boolean_condition(ast::expression &e,
                                              script &out_script,
                                              ast_factory &factory,
                                              constant_table const &constants,
                                              verb_table const &verbs);

        void visit_and_fold_array_subscript(ast::expression &e,
                                            script &out_script,
                                            ast_factory &factory,
                                            constant_table const &constants,
                                            verb_table const &verbs);
    }
}
