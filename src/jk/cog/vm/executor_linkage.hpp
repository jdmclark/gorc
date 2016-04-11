#pragma once

#include "utility/flag_set.hpp"
#include "jk/cog/script/value.hpp"
#include "jk/cog/script/source_type.hpp"
#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"

namespace gorc {
    namespace cog {

        class executor_linkage {
        public:
            flag_set<source_type> mask;
            cog_id instance_id;
            value sender_link_id;

            executor_linkage(flag_set<source_type> mask,
                             cog_id instance_id,
                             value sender_link_id);
            executor_linkage(deserialization_constructor_tag, binary_input_stream &);

            void binary_serialize_object(binary_output_stream &) const;
        };

    }
}
