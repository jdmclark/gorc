#pragma once

#include "ast.hpp"
#include "value.hpp"

namespace gorc {

    class assign_lvalue_visitor {
    private:
        shvalue value;

    public:
        assign_lvalue_visitor(shvalue const &value);

        void visit(variable_name &var) const;
        void visit(environment_variable_name &var) const;
    };

}
