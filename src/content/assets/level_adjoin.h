#pragma once

#include "content/flags/adjoin_flag.h"

namespace gorc {
namespace content {
namespace assets {

class level_adjoin {
public:
    flag_set<flags::adjoin_flag> flags;
    size_t mirror;
    float distance;
};

}
}
}
