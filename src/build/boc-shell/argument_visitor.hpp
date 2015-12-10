#pragma once

#include "ast.hpp"
#include "sexpr/sexpr.hpp"

namespace gorc {

    class argument_visitor {
    public:
        sexpr visit(argument &) const;
        sexpr visit(ast_list_node<argument*> &) const;
    };

}
