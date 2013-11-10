#pragma once

#include "content/constants.h"

namespace gorc {

// number of times per second the game state is updated.
const double gameplay_tick = 1.0 / 60.0;

// constant multiple to convert per-frame rate parameters.
const double rate_factor = 1.0 / 10.0;

}
