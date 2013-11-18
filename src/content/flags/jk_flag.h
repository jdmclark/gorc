#pragma once

namespace gorc {
namespace flags {

enum class jk_flag {
	has_saber						= 0x1,
	saber_collides					= 0x2,
	saber_igniting					= 0x4,
	saber_shrinking					= 0x8,
	has_two_sabers					= 0x10,
	persuasion_twinkling			= 0x20,
	attacking_in_same_keyframe		= 0x40
};

}
}
