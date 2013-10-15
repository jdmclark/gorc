#pragma once

#include "content/constants.h"

namespace gorc {

// number of times per second the game state is updated.
const double gameplay_tick = (1.0 / 60.0);

// Constant multiple to convert per-frame rate parameters.
const double rate_factor = 1.0 / 10.0;

// Constant factor to convert degrees to radians.
const double deg2rad = 0.0174532925;


}
