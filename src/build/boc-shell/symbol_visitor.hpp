#pragma once

#include "ast.hpp"

namespace gorc {

    class symbol_visitor {
    private:
        bool functions_allowed = false;

    public:
        symbol_visitor(bool functions_allowed);

        void visit(command_statement &) const;
        void visit(assignment_statement &) const;
        void visit(var_declaration_statement &) const;
        void visit(func_declaration_statement &) const;
        void visit(if_statement &) const;
        void visit(if_else_statement &) const;
        void visit(for_statement &) const;
        void visit(return_statement &) const;
        void visit(call_statement &) const;

        void visit(ast_list_node<statement*> &) const;
        void visit(compound_statement &) const;
        void visit(translation_unit &) const;
    };

}
