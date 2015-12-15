#pragma once

#include "ast.hpp"
#include "sexpr/sexpr.hpp"

namespace gorc {

    class program_visitor {
    public:
        maybe<sexpr> return_value;

        int visit(pipe_command &);
        int visit(infix_command &);

        void visit(command_statement &);

        void visit(assignment_statement &);
        void visit(var_declaration_statement &);
        void visit(if_statement &);
        void visit(if_else_statement &);
        void visit(for_statement &);
        void visit(return_statement &);
        void visit(call_statement &);

        void visit(ast_list_node<statement*> &);
        void visit(compound_statement &);
        void visit(translation_unit &);

        void visit(func_declaration_statement &);
    };

}
