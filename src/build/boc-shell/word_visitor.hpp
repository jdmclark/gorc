#pragma once

#include "ast.hpp"
#include "sexpr/sexpr.hpp"

namespace gorc {

    class word_visitor {
    public:
        sexpr visit(simple_word &) const;
        sexpr visit(variable_name &) const;
        sexpr visit(environment_variable_name &) const;
        sexpr visit(expression_word &) const;
    };

}
