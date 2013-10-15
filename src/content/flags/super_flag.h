#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class SuperFlag : uint32_t {
	WeaponSupercharge				= 0x1,
	ShieldSupercharge				= 0x2,
	ForceBoost						= 0x4
};

}
}
