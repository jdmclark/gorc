#pragma once

namespace gorc {

    enum class inventory_flag {
        registered              = 0x1,
        item                    = 0x2,
        weapon                  = 0x4,
        force_power             = 0x8,
        armored                 = 0x10,
        available_by_default    = 0x20,
        reset_between_levels    = 0x40,
        drops_on_death          = 0x80,
        hotkeyable              = 0x100
    };

}
