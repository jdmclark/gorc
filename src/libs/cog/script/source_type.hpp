#pragma once

namespace gorc {
    namespace cog {

        enum class source_type {
            free = 0x1,
            camera = 0x2,
            actor = 0x4,
            weapon = 0x8,
            debris = 0x10,
            item = 0x20,
            explosion = 0x40,
            cog = 0x80,
            ghost = 0x100,
            corpse = 0x200,
            player = 0x400,
            particle = 0x800
        };

    }
}
