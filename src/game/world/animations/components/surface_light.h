#pragma once

#include "base/utility/entity_id.h"
#include "base/io/binary_input_stream.h"
#include "base/io/binary_output_stream.h"
#include "content/flags/anim_flag.h"
#include "base/utility/flag_set.h"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace components {

class surface_light {
public:
    #include "surface_light.uid"

    int surface;
    float start_light, end_light, change_time, anim_time;

    surface_light(int surface, float start_light, float end_light, float change_time);
    surface_light(io::deserialization_constructor_tag, io::binary_input_stream& is);

    void serialize(io::binary_output_stream& os) const;
};

}
}
}
}
}
