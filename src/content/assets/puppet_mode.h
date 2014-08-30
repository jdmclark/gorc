#pragma once

#include "content/flags/puppet_submode_type.h"
#include "puppet_submode.h"
#include "base/utility/enum_hasher.h"
#include "base/utility/maybe.h"
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
