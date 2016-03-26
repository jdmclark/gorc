#pragma once

#include "type.hpp"
#include "value.hpp"
#include "source_type.hpp"
#include "utility/flag_set.hpp"
#include <string>

namespace gorc {
    namespace cog {

        class symbol {
        public:
            size_t sequence_number;

            value_type type;
            std::string name;

            value default_value;

            bool local;
            std::string desc;
            flag_set<source_type> mask;
            int link_id;
            bool no_link;

            symbol(size_t sequence_number,
                   std::string const &name);

            symbol(size_t sequence_number,
                   value_type type,
                   std::string const &name,
                   value default_value,
                   bool local,
                   std::string const &desc,
                   flag_set<source_type> mask,
                   int linkid,
                   bool nolink);
        };

    }
}
