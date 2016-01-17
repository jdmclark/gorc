#pragma once

#include "ast.hpp"
#include "value.hpp"
#include <vector>

namespace gorc {

    class program_visitor {
    public:
        maybe<shvalue> return_value;

        void visit(command_statement &);

        void visit(assignment_statement &);
        void visit(var_declaration_statement &);
        void visit(if_statement &);
        void visit(if_else_statement &);
        void visit(for_statement &);
        void visit(return_statement &);
        void visit(call_statement &);
        void visit(pushd_statement &);
        void visit(popd_statement &);

        void visit(ast_list_node<statement*> &);
        void visit(include_statement &);
        void visit(compound_statement &);
        void visit(translation_unit &);

        void visit(func_declaration_statement &);
    };

}
