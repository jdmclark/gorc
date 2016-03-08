#pragma once

namespace gorc {
namespace flags {

enum class ai_mode_flag {
    moving_toward_destination        = 0x1,
    attacking_target                = 0x2,
    searching_for_target            = 0x4,
    turning_to_face_target            = 0x8,
    has_move_target                    = 0x10,
    has_fire_target                    = 0x20,
    ready                            = 0x200,
    has_line_of_sight                = 0x400,
    fleeing                            = 0x800,
    passive                            = 0x1000,
    disabled                        = 0x2000
};

}
}
