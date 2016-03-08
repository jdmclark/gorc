#pragma once

#include "libold/content/flags/puppet_submode_type.hpp"
#include "puppet_submode.hpp"
#include "libold/base/utility/enum_hasher.hpp"
#include "utility/maybe.hpp"
#include <unordered_map>
#include <array>

namespace gorc {
namespace content {
namespace assets {

class puppet_mode {
public:
    std::unordered_map<flags::puppet_submode_type,
                       puppet_submode,
                       enum_hasher<flags::puppet_submode_type>> submodes;

    inline maybe<puppet_submode const*> get_submode(flags::puppet_submode_type type) const {
        auto it = submodes.find(type);
        if(it != submodes.end()) {
            return make_maybe(&it->second);
        }

        return nothing;
    }
};

}
}
}
