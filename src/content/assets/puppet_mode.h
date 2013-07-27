#pragma once

#include "content/flags/puppetsubmodetype.h"
#include "puppet_submode.h"
#include <unordered_map>
#include <array>

namespace Gorc {
namespace Content {
namespace Assets {

class PuppetMode {
public:
	std::array<PuppetSubmode, 64> Submodes;

	inline const PuppetSubmode& GetSubmode(Flags::PuppetSubmodeType type) const {
		return Submodes[static_cast<unsigned int>(type)];
	}
};

}
}
}
