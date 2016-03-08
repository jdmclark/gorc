#pragma once

namespace gorc {
namespace flags {

enum class weapon_flag {
    does_not_affect_parent = 0x1,
    explodes_on_surface = 0x4,
    explodes_on_thing = 0x8,
    attaches_to_surface = 0x80,
    timed_explosion = 0x100,
    explodes_when_damaged = 0x200,
    ricochets_on_magsealed = 0x400,
    attaches_to_thing = 0x800,
    explode_soon_after_colliding_with_thing = 0x1000,
    explodes_soon_after_creation = 0x2000,
    uses_damage_decay = 0x4000,
    has_trailthings = 0x8000,
    explodes_soon_after_creation_2 = 0x10000,

    ricochets_on_surface = 0x80000
};

}
}
