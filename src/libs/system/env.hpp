#pragma once

#include "utility/maybe.hpp"
#include <string>

namespace gorc {
    bool has_environment_variable(std::string const &);
    maybe<std::string> get_environment_variable(std::string const &);
    void set_environment_variable(std::string const &key, std::string const &value);
}
