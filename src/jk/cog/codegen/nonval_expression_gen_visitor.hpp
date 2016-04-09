#pragma once

#include "jk/cog/ast/ast.hpp"
#include "jk/cog/script/script.hpp"
#include "jk/cog/ir/ir_printer.hpp"
#include "jk/cog/script/verb_table.hpp"
#include "jk/cog/script/constant_table.hpp"

namespace gorc {
    namespace cog {

        class nonval_expression_gen_visitor {
        private:
            script &out_script;
            ir_printer &ir;
            verb_table const &verbs;
            constant_table const &constants;

        public:
            nonval_expression_gen_visitor(script &out_script,
                                          ir_printer &ir,
                                          verb_table const &verbs,
                                          constant_table const &constants);

            void visit(ast::immediate_expression &);
            void visit(ast::string_literal_expression &);
            void visit(ast::integer_literal_expression &);
            void visit(ast::float_literal_expression &);
            void visit(ast::vector_literal_expression &);
            void visit(ast::identifier_expression &);
            void visit(ast::subscript_expression &);
            void visit(ast::method_call_expression &);
            void visit(ast::unary_expression &);
            void visit(ast::infix_expression &);
            void visit(ast::assignment_expression &);
            void visit(ast::comma_expression &);

            void visit(ast::for_empty_expression &);
            void visit(ast::for_expression &);
        };

    }
}
