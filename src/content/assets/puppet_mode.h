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
	std::array<puppet_submode, 64> submodes;

	inline const puppet_submode& get_submode(flags::puppet_submode_type type) const {
		return submodes[static_cast<unsigned int>(type)];
	}
};

}
}
}
