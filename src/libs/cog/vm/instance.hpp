#pragma once

#include "content/asset_ref.hpp"
#include "cog/script/script.hpp"
#include "heap.hpp"
#include <vector>

namespace gorc {
    namespace cog {

        class instance {
        public:
            asset_ref<script> cog;
            heap memory;

            instance(asset_ref<script>);
            instance(asset_ref<script>, std::vector<value> const &);
        };

    }
}
