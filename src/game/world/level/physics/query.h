#pragma once

#include "content/assets/level.h"
#include "game/world/level/surface.h"
#include "shape.h"
#include "contact.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Physics {

bool PointInsideSurface(const Math::Vector<3>& point, const Content::Assets::Level& level, const Content::Assets::LevelSurface& surface);
bool SegmentSurfaceIntersection(const Segment& segment, const Content::Assets::Level& level, const Content::Assets::LevelSurface& surface);

Math::Vector<3> ClosestPointOnSurface(const Math::Vector<3>& origin, const Content::Assets::Level& level, const Content::Assets::LevelSurface& surface);

}
}
}
}
}
