#pragma once

#include "cog/ast/ast.hpp"

namespace gorc {

    class print_ast_visitor {
    private:
        int indentation = 0;
        void print_tabs();

    public:
        void visit(cog::ast::list_node<cog::ast::statement*> &);
        void visit(cog::ast::compound_statement &);
        void visit(cog::ast::empty_statement &);
        void visit(cog::ast::expression_statement &);
        void visit(cog::ast::break_statement &);
        void visit(cog::ast::return_statement &);
        void visit(cog::ast::call_statement &);
        void visit(cog::ast::if_statement &);
        void visit(cog::ast::if_else_statement &);
        void visit(cog::ast::while_statement &);
        void visit(cog::ast::do_statement &);
        void visit(cog::ast::for_statement &);
        void visit(cog::ast::labeled_statement &);

        void visit(cog::ast::immediate_expression &);
        void visit(cog::ast::string_literal_expression &);
        void visit(cog::ast::integer_literal_expression &);
        void visit(cog::ast::float_literal_expression &);
        void visit(cog::ast::vector_literal_expression &);
        void visit(cog::ast::identifier_expression &);
        void visit(cog::ast::subscript_expression &);
        void visit(cog::ast::method_call_expression &);
        void visit(cog::ast::unary_expression &);
        void visit(cog::ast::infix_expression &);
        void visit(cog::ast::assignment_expression &);
        void visit(cog::ast::comma_expression &);

        void visit(cog::ast::for_empty_expression &);
        void visit(cog::ast::for_expression &);
    };

}
