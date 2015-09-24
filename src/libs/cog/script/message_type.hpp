#pragma once

#include <cstdint>
#include "constant_table.hpp"

namespace gorc {
    namespace cog {

        enum class message_type : uint8_t {
            global0             = 0,
            activated           = 1,
            removed             = 2,
            startup             = 3,
            timer               = 4,
            blocked             = 5,
            entered             = 6,
            exited              = 7,
            crossed             = 8,
            sighted             = 9,
            damaged             = 10,
            arrived             = 11,
            killed              = 12,
            pulse               = 13,
            touched             = 14,
            created             = 15,
            loading             = 16,
            selected            = 17,
            deselected          = 18,
            autoselect          = 19,
            changed             = 20,
            deactivated         = 21,
            shutdown            = 22,
            respawn             = 23,
            ai_event            = 24,
            skill               = 25,
            taken               = 26,
            user0               = 27,
            user1               = 28,
            user2               = 29,
            user3               = 30,
            user4               = 31,
            user5               = 32,
            user6               = 33,
            user7               = 34,
            new_player          = 35,
            fire                = 36,
            join                = 37,
            leave               = 38,
            splash              = 39,
            trigger             = 40
        };

        char const* as_string(message_type);

        void add_messages_to_constant_table(constant_table &table);
    }
}
