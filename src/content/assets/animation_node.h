#pragma once

#include "animation_frame.h"
#include <vector>

namespace gorc {
namespace content {
namespace assets {

class animation_node {
public:
    std::vector<animation_frame> frames;
};

}
}
}
