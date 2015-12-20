#pragma once

#include "ast.hpp"
#include "value.hpp"

namespace gorc {

    class word_visitor {
    public:
        shvalue visit(simple_word &) const;
        shvalue visit(variable_name &) const;
        shvalue visit(environment_variable_name &) const;
        shvalue visit(expression_word &) const;
    };

}
