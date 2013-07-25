#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class SuperFlag : uint32_t {
	WeaponSupercharge				= 0x1,
	ShieldSupercharge				= 0x2,
	ForceBoost						= 0x4
};

}
}
