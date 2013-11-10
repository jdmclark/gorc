#pragma once

namespace gorc {
namespace flags {

enum class adjoin_flag {
	Visible 						= 0x1,
	AllowMovement 					= 0x2,
	AllowSound 						= 0x4,
	AllowPlayerOnly 				= 0x8,
	AllowAiOnly 					= 0x10
};

}
}
