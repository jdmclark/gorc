#pragma once

#include "libold/base/utility/entity_id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace events {

class killed {
public:
    #include "killed.uid"

    int thing;
    int killer;

    killed(int thing, int killer);
};

}
}
}
}
