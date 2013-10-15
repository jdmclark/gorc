#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class ItemFlag : uint32_t {
	RespawnInMultiplayer			= 0x1,
	RespawnInSingleplayer			= 0x2,
	Backpack						= 0x4,
	CtfFlag							= 0x8
};

}
}
