#pragma once

#include "material_cel.hpp"
#include "libold/content/loaders/material_loader.hpp"
#include <vector>

namespace gorc {
namespace content {
namespace assets {

class material : public asset {
public:
    static fourcc const type;

    box<2, int> size;
    std::vector<material_cel> cels;
};

}
}
}
