#pragma once

#include "cog/script/script.hpp"
#include "cog/ast/ast.hpp"
#include "collect_labels.hpp"
#include <string>

namespace gorc {
    namespace cog {

        class flow_visitor {
        private:
            script &out_script;
            seen_label_set const &called_labels;

            bool inside_dead_code;
            bool seen_dead_code;
            bool inside_loop;
            diagnostic_context_location dead_code_location;

            std::string inside_label;

            void add_to_dead_code(diagnostic_context_location const &e);
            void end_dead_code();

            void check_flow_out(std::string const &out_label);

        public:
            flow_visitor(script &out_script,
                         seen_label_set const &called_labels,
                         bool inside_dead_code,
                         bool inside_loop,
                         std::string const &inside_label);
            ~flow_visitor();

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
