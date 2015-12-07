#pragma once

#include "ast.hpp"

namespace gorc {

    class program_visitor {
    public:
        int visit(pipe_command &) const;
        int visit(command_statement &) const;
        int visit(assignment_statement &) const;
        int visit(var_declaration_statement &) const;
        int visit(if_statement &) const;
        int visit(if_else_statement &) const;

        int visit(ast_list_node<statement*> &) const;
        int visit(compound_statement &) const;
        int visit(translation_unit &) const;
    };

}
