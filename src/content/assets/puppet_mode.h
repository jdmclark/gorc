#pragma once

#include "content/flags/puppet_submode_type.h"
#include "puppet_submode.h"
#include <unordered_map>
#include <array>

namespace gorc {
namespace content {
namespace assets {

class puppet_mode {
public:
    std::unordered_map<int, puppet_submode> submodes;

    inline const puppet_submode& get_submode(flags::puppet_submode_type type) const {
        if(submodes.find(static_cast<unsigned int>(type)) == submodes.end())
            return submodes.at(0); //Return idle by default if submode is nonexistant
        return submodes.at(static_cast<unsigned int>(type));
    }

    inline const int submode_exists(flags::puppet_submode_type type) const {
        if(submodes.find(static_cast<unsigned int>(type)) == submodes.end() || submodes.find(0) == submodes.end())
            return 0;
        return 1;
    }
};

}
}
}
