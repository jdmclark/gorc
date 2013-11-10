#pragma once

namespace gorc {
namespace flags {

enum class physics_flag {
	has_gravity						= 0x1,
	uses_thrust_to_move				= 0x2,
	align_orientation_with_surface	= 0x10,
	bounce_off_surface				= 0x20,
	sticks_to_floor					= 0x40,
	sticks_to_wall					= 0x80,
	uses_rotational_velocity		= 0x200,
	banks_when_turning				= 0x400,
	uses_angular_thrust_to_rotate	= 0x1000,
	can_fly							= 0x2000,
	affected_by_blast_force			= 0x4000,
	is_crouching					= 0x10000,
	do_not_rotate_velocity			= 0x20000,
	partial_gravity					= 0x40000,
	treading_water					= 0x100000,
	not_affected_by_thrust			= 0x400000
};

}
}
