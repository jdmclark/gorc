#pragma once

#include "content/asset_ref.hpp"
#include "cog/script/script.hpp"
#include "heap.hpp"
#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"
#include <vector>

namespace gorc {
    namespace cog {

        class instance {
        public:
            asset_ref<script> cog;
            heap memory;

            explicit instance(asset_ref<script>);
            instance(asset_ref<script>, std::vector<value> const &);

            instance(deserialization_constructor_tag, binary_input_stream &);
            void binary_serialize_object(binary_output_stream &) const;
        };

    }
}
