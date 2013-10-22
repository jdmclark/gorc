#pragma once

namespace gorc {
namespace flags {

enum class DamageFlag {
	Impact							= 0x1,
	Energy							= 0x2,
	Explosion						= 0x4,
	Force							= 0x8,
	Saber							= 0x10,
	Drowning						= 0x20
};

}
}
