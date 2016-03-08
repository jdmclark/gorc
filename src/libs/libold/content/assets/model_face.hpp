#pragma once

#include "utility/flag_set.hpp"
#include "libold/content/flags/face_flag.hpp"
#include "libold/content/flags/geometry_mode.hpp"
#include "libold/content/flags/light_mode.hpp"
#include "libold/content/flags/texture_mode.hpp"

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
