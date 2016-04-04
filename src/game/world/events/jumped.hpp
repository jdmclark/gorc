#pragma once

#include "libold/base/utility/entity_id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace events {

class jumped {
public:
    #include "jumped.uid"

    int thing;

    jumped(int thing);
};

}
}
}
}
