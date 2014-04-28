#pragma once

#include "base/utility/entity_id.h"
#include "base/io/binary_input_stream.h"
#include "base/io/binary_output_stream.h"
#include "base/math/vector.h"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace components {

class slide_surface {
public:
    #include "slide_surface.uid"

    entity_id surface;
    vector<3> direction;

    vector<3> sb0, sb1;
    vector<2> tb0, tb1;

    slide_surface(entity_id surface, const vector<3>& direction, const vector<3>& sb0, const vector<3>& sb1,
            const vector<2>& tb0, const vector<2>& tb1);
    slide_surface(io::deserialization_constructor_tag, io::binary_input_stream& is);

    void serialize(io::binary_output_stream& os) const;
};

}
}
}
}
}
