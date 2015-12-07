#pragma once

#include "ast.hpp"

namespace gorc {

    class expression_visitor {
    public:
        std::string visit(argument_expression &) const;
        std::string visit(unary_expression &) const;
        std::string visit(infix_expression &) const;
    };

}
