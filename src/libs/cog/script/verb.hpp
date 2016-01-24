#pragma once

#include "value.hpp"
#include "stack.hpp"
#include "utility/service_registry.hpp"
#include <vector>
#include <string>

namespace gorc {
    namespace cog {

        class verb {
        public:
            std::string const name;
            value_type const return_type;
            std::vector<value_type> const argument_types;

            verb(std::string const &name,
                 value_type return_type,
                 std::vector<value_type> &&argument_types);
            virtual ~verb();

            virtual value invoke(stack &,
                                 service_registry &,
                                 bool expects_value) const = 0;
        };

    }
}
