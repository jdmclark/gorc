#pragma once

#include "ast.hpp"
#include "sexpr.hpp"

namespace gorc {

    class expression_visitor {
    public:
        sexpr visit(argument_expression &) const;
        sexpr visit(unary_expression &) const;
        sexpr visit(infix_expression &) const;
    };

}
