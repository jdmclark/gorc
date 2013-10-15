#pragma once

#include "content/assets/level.h"
#include "game/world/level/surface.h"
#include "shape.h"
#include "contact.h"

namespace gorc {
namespace game {
namespace world {
namespace level {
namespace physics {

bool point_inside_surface(const vector<3>& point, const content::assets::level& level, const content::assets::level_surface& surface);
bool segment_surface_intersection(const segment& segment, const content::assets::level& level, const content::assets::level_surface& surface);

vector<3> closest_point_on_surface(const vector<3>& origin, const content::assets::level& level, const content::assets::level_surface& surface);

}
}
}
}
}
