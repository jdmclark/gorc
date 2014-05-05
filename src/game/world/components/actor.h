#pragma once

#include "base/io/binary_input_stream.h"
#include "base/io/binary_output_stream.h"

namespace gorc {
namespace game {
namespace world {
namespace components {

class actor {
public:
    #include "actor.uid"

    actor() = default;
    actor(io::deserialization_constructor_tag, io::binary_input_stream&);
    void serialize(io::binary_output_stream&) const;
};

}
}
}
}
