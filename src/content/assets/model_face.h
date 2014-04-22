#pragma once

#include "base/utility/flag_set.h"
#include "content/flags/face_flag.h"
#include "content/flags/geometry_mode.h"
#include "content/flags/light_mode.h"
#include "content/flags/texture_mode.h"

#include <vector>
#include <tuple>

namespace gorc {
namespace content {
namespace assets {

class model_face {
public:
    int material;
    flag_set<flags::face_flag> type;
    flags::geometry_mode geo;
    flags::light_mode light;
    flags::texture_mode tex;
    float extra_light;
    std::vector<std::tuple<size_t, size_t>> vertices;
    vector<3> normal;
};

}
}
}
