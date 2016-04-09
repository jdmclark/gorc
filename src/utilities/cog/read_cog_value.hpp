#pragma once

#include "jk/cog/script/value.hpp"
#include "text/json_input_stream.hpp"

namespace gorc {
    cog::value read_cog_value(json_input_stream &f);
}
