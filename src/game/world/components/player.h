#pragma once

#include "base/io/binary_input_stream.h"
#include "base/io/binary_output_stream.h"

namespace gorc {
namespace game {
namespace world {
namespace components {

class player {
public:
    #include "player.uid"

    player() = default;
    player(io::deserialization_constructor_tag, io::binary_input_stream&);
    void serialize(io::binary_output_stream&) const;
};

}
}
}
}
