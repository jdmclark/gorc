#pragma once

#include "jk/content/colormap.hpp"
#include "utility/maybe.hpp"

namespace gorc {
namespace content {

    class master_colormap {
    public:
        maybe<asset_ref<colormap>> cmp;
    };

}
}
