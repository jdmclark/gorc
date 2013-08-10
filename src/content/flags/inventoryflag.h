#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class InventoryFlag : uint32_t {
	Registered						= 0x1,
	Item							= 0x2,
	Weapon							= 0x4,
	ForcePower						= 0x8,
	Armored							= 0x10,
	AvailableByDefault				= 0x20,
	ResetBetweenLevels				= 0x40,
	DropsOnDeath					= 0x80,
	Hotkeyable						= 0x100
};

}
}
