#pragma once

#include <string>
#include <unordered_set>
#include "utility/enum_hash.hpp"

namespace gorc {

    enum class external_lib_type {
        boost
    };

    external_lib_type to_external_lib_type(std::string const &);

    using ext_lib_set = std::unordered_set<external_lib_type, enum_hash<external_lib_type>>;

    class entity_input_stream;
    ext_lib_set read_ext_lib_set(entity_input_stream &);

    class entity_output_stream;
    void write_ext_lib_set(ext_lib_set const &, entity_output_stream &);

}
