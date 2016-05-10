#pragma once

#include "content/asset.hpp"
#include "math/size.hpp"

namespace gorc {

    class material : public asset {
    public:
        static fourcc const type;

        std::vector<size<2, int>> cels;
    };

    template <>
    struct id_asset_type<material_id> {
        using type = material;
    };

}
