#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class ExplosionFlag : uint32_t {
	AnimatedSprite					= 0x1,
	ExplosionIncludesBlastPhase		= 0x2,
	DamageInBlastRadius				= 0x4,
	HasChildExplosions				= 0x8,
	VariableLight					= 0x10,
	NoDamageToShooter				= 0x40,
	RandomDebris					= 0x80,
	FlashBlindsThings				= 0x100
};

}
}
