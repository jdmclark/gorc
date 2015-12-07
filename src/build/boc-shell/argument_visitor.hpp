#pragma once

#include "ast.hpp"

namespace gorc {

    class argument_visitor {
    public:
        std::string visit(argument &) const;
    };

}
