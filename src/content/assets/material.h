#pragma once

#include "material_cel.h"
#include "content/loaders/material_loader.h"
#include <vector>

namespace gorc {
namespace content {
namespace assets {

class material : public asset {
public:
    using loader = loaders::material_loader;

    box<2, int> size;
    std::vector<material_cel> cels;
};

}
}
}
