#pragma once

#include "ast.hpp"
#include "value.hpp"

namespace gorc {

    class expression_visitor {
    public:
        shvalue visit(argument_expression &) const;
        shvalue visit(unary_expression &) const;
        shvalue visit(infix_expression &) const;
        shvalue visit(nil_expression &) const;
        shvalue visit(call_expression &) const;
    };

}
