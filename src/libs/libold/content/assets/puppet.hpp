#pragma once

#include "content/asset.hpp"
#include "puppet_mode.hpp"
#include "libold/content/loaders/puppet_loader.hpp"
#include "libold/content/flags/puppet_mode_type.hpp"
#include "libold/content/flags/puppet_joint_type.hpp"
#include <array>
#include <unordered_map>

namespace gorc {
namespace content {
namespace assets {

class puppet : public asset {
public:
    using loader = loaders::puppet_loader;

    std::array<puppet_mode, 6> modes;
    std::unordered_map<int, int> joint_map;

    inline const puppet_mode& get_mode(flags::puppet_mode_type mode) const {
        return modes[static_cast<unsigned int>(mode)];
    }

    inline int get_joint(flags::puppet_joint_type type) const {
        auto it = joint_map.find(static_cast<int>(type));
        if(it == joint_map.end()) {
            return -1;
        }
        else {
            return it->second;
        }
    }
};

}
}
}
