#pragma once

#include "content/asset_ref.hpp"
#include "jk/cog/script/script.hpp"
#include "heap.hpp"
#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"
#include "jk/cog/script/source_type.hpp"
#include "utility/flag_set.hpp"
#include <vector>

namespace gorc {
    namespace cog {

        class instance_linkage {
        public:
            value object;
            flag_set<source_type> mask;
            value sender_link_id;

            instance_linkage(value object,
                             flag_set<source_type> mask,
                             value sender_link_id);
            instance_linkage(deserialization_constructor_tag, binary_input_stream &);

            void binary_serialize_object(binary_output_stream &) const;
        };

        class instance {
        public:
            asset_ref<script> cog;
            heap memory;
            std::vector<instance_linkage> linkages;

            explicit instance(asset_ref<script>);
            instance(asset_ref<script>, std::vector<value> const &);

            instance(deserialization_constructor_tag, binary_input_stream &);
            void binary_serialize_object(binary_output_stream &) const;
        };

    }
}
