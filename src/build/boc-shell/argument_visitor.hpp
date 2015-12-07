#pragma once

#include "ast.hpp"
#include "sexpr.hpp"

namespace gorc {

    class argument_visitor {
    public:
        sexpr visit(argument &) const;
        sexpr visit(ast_list_node<argument*> &) const;
    };

    std::vector<std::string> argument_list_to_argv(sexpr);

}
