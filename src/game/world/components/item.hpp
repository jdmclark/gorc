#pragma once

#include <cstdint>

namespace gorc {
namespace game {
namespace world {
namespace components {

class item {
public:
    #include "item.uid"

    item() = default;
};

}
}
}
}
