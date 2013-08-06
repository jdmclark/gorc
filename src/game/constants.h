#pragma once

namespace Gorc {
namespace Game {

// Number of times per second the game state is updated.
const double GameplayTick = (1.0 / 120.0);

// Constant multiple to convert per-frame rate parameters.
const double RateFactor = (1.0 / 8.0);

}
}
