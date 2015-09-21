#pragma once

#include "message_type.hpp"
#include "utility/enum_hash.hpp"
#include "utility/maybe.hpp"
#include <unordered_map>

namespace gorc {
    namespace cog {

        class message_table {
        private:
            std::unordered_map<message_type, size_t, enum_hash<message_type>> offsets;

        public:
            void set_offset(message_type, size_t);
            maybe<size_t> get_offset(message_type) const;
        };

    }
}
