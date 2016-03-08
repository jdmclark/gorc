#pragma once

#include "libold/content/flags/adjoin_flag.hpp"

namespace gorc {
namespace content {
namespace assets {

class level_adjoin {
public:
    flag_set<flags::adjoin_flag> flags;
    int mirror;
    float distance;
};

}
}
}
