#pragma once

#include "ast.hpp"

namespace gorc {

    class word_visitor {
    public:
        std::string visit(simple_word &) const;
        std::string visit(variable_name &) const;
        std::string visit(environment_variable_name &) const;
    };

}
