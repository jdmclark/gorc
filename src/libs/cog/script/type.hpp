#pragma once

#include <cstdint>

namespace gorc {
    namespace cog {

        enum class value_type : uint8_t {

            // Special types
            nothing,
            dynamic,
            string,
            message,

            // Mathematical types
            boolean,
            integer,
            floating,
            vector,

            // Entity types
            sector,
            surface,
            thing,
            player,

            // Asset types
            ai,
            cog,
            colormap,
            keyframe,
            material,
            model,
            sound,
            thing_template
        };

        char const* as_string(value_type);

        bool is_id_type(value_type vt);
        bool is_scalar_type(value_type vt);
        bool is_truth_value_type(value_type vt);
        bool can_convert_type(value_type vt_initial, value_type vt_target);

        value_type get_closest_numeric_type(value_type value);
        value_type get_common_numeric_type(value_type left, value_type right);

    }
}
