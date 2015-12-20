#pragma once

#include "ast.hpp"
#include "value.hpp"

namespace gorc {

    class argument_visitor {
    public:
        shvalue visit(argument &) const;
        shvalue visit(ast_list_node<argument*> &) const;
    };

}
