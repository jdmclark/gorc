#pragma once

#include <cstdint>
#include "math/box.hpp"

namespace gorc {
namespace events {

class resized {
public:
    #include "resized.uid"

    box<2, int> view_size;

    resized(const box<2, int>& view_size);
};

}
}
