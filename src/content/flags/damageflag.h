#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class DamageFlag : uint32_t {
	Impact							= 0x1,
	Energy							= 0x2,
	Explosion						= 0x4,
	Force							= 0x8,
	Saber							= 0x10,
	Drowning						= 0x20
};

}
}
