#pragma once

#include "inventory_flag.hpp"
#include "jk/cog/script/script.hpp"
#include "utility/flag_set.hpp"
#include "utility/maybe.hpp"

#include <string>

namespace gorc {

    class inventory_bin {
    public:
        std::string name;
        int bin_id;
        int min_value;
        int max_value;
        flag_set<inventory_flag> flags;

        maybe<asset_ref<cog::script>> cog;
    };

}
