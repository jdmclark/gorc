#pragma once

#include "cog/ast/ast.hpp"
#include "ast/factory.hpp"
#include "cog/script/script.hpp"
#include "cog/script/constant_table.hpp"
#include "cog/script/verb_table.hpp"
#include "collect_labels.hpp"

namespace gorc {
    namespace cog {

        class code_visitor {
        private:
            script &out_script;
            ast_factory &factory;
            constant_table const &constants;
            verb_table const &verbs;
            seen_label_set const &labels;
            bool const inside_loop;
            bool const inside_block;

        public:
            code_visitor(script &out_script,
                         ast_factory &factory,
                         constant_table const &constants,
                         verb_table const &verbs,
                         seen_label_set const &labels,
                         bool inside_loop,
                         bool inside_block);

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
