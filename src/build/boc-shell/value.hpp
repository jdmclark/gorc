#pragma once

#include <vector>
#include <string>

namespace gorc {

    typedef std::vector<std::string> shvalue;

    shvalue shvalue_from_string(std::string const &);
    std::string shvalue_to_string(shvalue const &);

    shvalue shvalue_from_boolean(bool);
    bool shvalue_to_boolean(shvalue const &);

}
