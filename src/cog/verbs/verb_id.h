#pragma once

#include <cstddef>

namespace gorc {
namespace cog {
namespace verbs {

class verb_table;

class verb_id {
    friend class verb_table;

private:
    size_t id;

public:
    verb_id(size_t id);
};

}
}
}
