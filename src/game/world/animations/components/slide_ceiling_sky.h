#pragma once

#include "base/io/binary_input_stream.h"
#include "base/io/binary_output_stream.h"
#include "base/math/vector.h"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace components {

class slide_ceiling_sky {
public:
    #include "slide_ceiling_sky.uid"

    vector<2> speed;

    slide_ceiling_sky(const vector<2>& speed);
    slide_ceiling_sky(io::deserialization_constructor_tag, io::binary_input_stream& is);

    void serialize(io::binary_output_stream& os) const;
};

}
}
}
}
}
