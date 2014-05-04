#pragma once

#include "base/utility/entity_id.h"
#include "base/io/binary_input_stream.h"
#include "base/io/binary_output_stream.h"
#include "content/flags/anim_flag.h"
#include "base/utility/flag_set.h"

namespace gorc {
namespace game {
namespace world {

class level_model;

namespace animations {
namespace components {

class surface_material {
public:
    #include "surface_material.uid"

    int surface;
    double framerate;
    flag_set<flags::anim_flag> flag;
    double framerate_accumulator = 0.0;

    surface_material(int surface, double framerate, flag_set<flags::anim_flag> flag);
    surface_material(io::deserialization_constructor_tag, io::binary_input_stream& is);

    void serialize(io::binary_output_stream& os) const;
};

}
}
}
}
}
