#pragma once

#include <string>
#include <unordered_set>
#include "utility/enum_hash.hpp"

namespace gorc {

    enum class external_lib_type {
        boost,
        sfml
    };

    external_lib_type to_external_lib_type(std::string const &);

    using ext_lib_set = std::unordered_set<external_lib_type, enum_hash<external_lib_type>>;
}
