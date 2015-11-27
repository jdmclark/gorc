#pragma once

#include <unordered_set>
#include <string>
#include "cog/ast/ast.hpp"

namespace gorc {
    namespace cog {

        using seen_label_set = std::unordered_set<std::string>;

        class collect_labels_visitor {
        private:
            seen_label_set &seen_labels;
            seen_label_set &called_labels;

        public:
            collect_labels_visitor(seen_label_set &seen_labels,
                                   seen_label_set &called_labels);

            void visit(ast::identifier&);

            void visit(ast_list_node<ast::statement*>&);
            void visit(ast::compound_statement&);
            void visit(ast::empty_statement&);
            void visit(ast::expression_statement&);
            void visit(ast::break_statement&);
            void visit(ast::return_statement&);
            void visit(ast::call_statement&);
            void visit(ast::if_statement&);
            void visit(ast::if_else_statement&);
            void visit(ast::while_statement&);
            void visit(ast::do_statement&);
            void visit(ast::for_statement&);
            void visit(ast::labeled_statement&);
        };

    }
}
