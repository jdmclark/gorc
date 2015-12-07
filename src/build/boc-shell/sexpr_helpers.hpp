#pragma once

#include "sexpr.hpp"
#include <string>

namespace gorc {

    std::string as_string_value(sexpr);
    bool as_boolean_value(sexpr);
    bool sexpr_equal(sexpr, sexpr);

}
