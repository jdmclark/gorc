#pragma once

#include "jk/cog/ast/ast.hpp"
#include "jk/cog/script/script.hpp"
#include "jk/cog/script/constant_table.hpp"
#include "jk/cog/script/verb_table.hpp"
#include "jk/cog/ir/ir_printer.hpp"
#include <stack>

namespace gorc {
    namespace cog {

        class statement_gen_visitor {
        private:
            script &out_script;
            ir_printer &ir;
            verb_table const &verbs;
            constant_table const &constants;

            std::stack<label_id> break_labels;

        public:
            statement_gen_visitor(script &out_script,
                                  ir_printer &ir,
                                  verb_table const &verbs,
                                  constant_table const &constants);

            void visit(ast::compound_statement &);
            void visit(ast::empty_statement &);
            void visit(ast::expression_statement &);
            void visit(ast::break_statement &);
            void visit(ast::return_statement &);
            void visit(ast::call_statement &);
            void visit(ast::if_statement &);
            void visit(ast::if_else_statement &);
            void visit(ast::while_statement &);
            void visit(ast::do_statement &);
            void visit(ast::for_statement &);
            void visit(ast::labeled_statement &);

            void visit(ast_list_node<ast::statement*> &);
        };

    }
}
