#pragma once

#include "base/io/binary_input_stream.h"
#include "base/io/binary_output_stream.h"
#include "base/utility/entity_id.h"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace components {

class surface_animation {
public:
    #include "surface_animation.uid"

    int surface;

    surface_animation(int surface);
    surface_animation(io::deserialization_constructor_tag, io::binary_input_stream&);

    void serialize(io::binary_output_stream&) const;
};

}
}
}
}
}
