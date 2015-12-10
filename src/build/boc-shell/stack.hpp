#pragma once

#include <string>
#include <unordered_map>
#include "sexpr/sexpr.hpp"

namespace gorc {

    class stack_frame {
    public:
        std::unordered_map<std::string, sexpr> variable_map;
    };

    class scoped_stack_frame {
    public:
        scoped_stack_frame();
        ~scoped_stack_frame();
    };

    void create_variable(std::string const &name, sexpr);
    sexpr get_variable_value(std::string const &name);
    void set_variable_value(std::string const &name, sexpr);
}
