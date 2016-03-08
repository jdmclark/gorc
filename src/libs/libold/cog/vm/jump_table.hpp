#pragma once

#include "libold/cog/message_id.hpp"
#include <vector>
#include <cstddef>

namespace gorc {
namespace cog {
namespace vm {

class jump_table {
private:
    std::vector<size_t> table;

public:
    jump_table();

    inline void set_target(message_id id, size_t target) {
        table[static_cast<uint8_t>(id)] = target;
    }

    inline size_t get_target(message_id id) const {
        return table[static_cast<uint8_t>(id)];
    }
};

}
}
}

