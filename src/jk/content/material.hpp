#pragma once

#include "content/asset.hpp"
#include "math/size.hpp"

namespace gorc {

    class material : public asset {
    public:
        static fourcc const type;

        std::vector<size<2, int>> cels;
    };

    class content_manager;
    asset_ref<material> get_asset(content_manager &cm, material_id);

}
