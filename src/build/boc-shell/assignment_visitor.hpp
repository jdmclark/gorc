#pragma once

#include "ast.hpp"
#include "sexpr/sexpr.hpp"

namespace gorc {

    class assign_lvalue_visitor {
    private:
        sexpr value;

    public:
        assign_lvalue_visitor(sexpr value);

        void visit(variable_name &var) const;
        void visit(environment_variable_name &var) const;
    };

}
