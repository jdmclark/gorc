#pragma once

#include "sexpr.hpp"
#include <string>
#include <vector>

namespace gorc {

    std::string as_string_value(sexpr);
    bool as_boolean_value(sexpr);
    bool sexpr_equal(sexpr, sexpr);

    std::vector<std::string> argument_to_argv(sexpr);
    std::vector<std::string> argument_list_to_argv(sexpr);

}
