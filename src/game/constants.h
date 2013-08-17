#pragma once

#include "content/constants.h"

namespace Gorc {

// Number of times per second the game state is updated.
const double GameplayTick = (1.0 / 120.0);

// Constant multiple to convert per-frame rate parameters.
const double RateFactor = 1.0 / 10.0;

// Constant factor to convert degrees to radians.
const double Deg2Rad = 0.0174532925;


}
