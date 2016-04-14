#pragma once

#include <string>
#include "jk/cog/script/value.hpp"

namespace gorc {
    namespace cog {

        class default_value_mapping {
        public:
            virtual ~default_value_mapping();

            virtual value operator()(value_type type,
                                     std::string const &value) const = 0;
        };

    }
}
