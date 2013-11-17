#pragma once

#include "framework/content/asset.h"
#include "puppet_mode.h"
#include "content/loaders/puppet_loader.h"
#include "content/flags/puppet_mode_type.h"
#include "content/flags/puppet_joint_type.h"
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
