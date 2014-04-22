#pragma once

#include <cstdint>
#include "base/math/box.h"

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
