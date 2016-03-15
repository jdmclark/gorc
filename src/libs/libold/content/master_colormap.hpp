#pragma once

#include "assets/colormap.hpp"
#include "utility/maybe.hpp"

namespace gorc {
namespace content {

    class master_colormap {
    public:
        maybe<asset_ref<assets::colormap>> cmp;
    };

}
}
