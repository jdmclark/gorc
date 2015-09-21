#pragma once

#include "cog/ast/ast.hpp"
#include "cog/script/script.hpp"
#include "cog/script/verb_table.hpp"
#include "cog/script/constant_table.hpp"
#include "cog/ir/ir_printer.hpp"

namespace gorc {
    namespace cog {

        class lval_expression_gen_visitor {
        private:
            script &out_script;
            ir_printer &ir;
            verb_table const &verbs;
            constant_table const &constants;

        public:
            lval_expression_gen_visitor(script &out_script,
                                        ir_printer &ir,
                                        verb_table const &verbs,
                                        constant_table const &constants);

            void visit(ast::node &);
            void visit(ast::identifier_expression &);
            void visit(ast::subscript_expression &);
        };

    }
}
