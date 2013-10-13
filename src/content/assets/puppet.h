#pragma once

#include "content/asset.h"
#include "puppet_mode.h"
#include "content/loaders/puppet_loader.h"
#include "content/flags/puppet_mode_type.h"
#include <array>
#include <unordered_map>

namespace Gorc {
namespace Content {
namespace Assets {

class Puppet : public Asset {
public:
	using Loader = Loaders::PuppetLoader;

	std::array<PuppetMode, 6> Modes;
	std::unordered_map<int, int> JointMap;

	inline const PuppetMode& GetMode(Flags::PuppetModeType mode) const {
		return Modes[static_cast<unsigned int>(mode)];
	}
};

}
}
}
