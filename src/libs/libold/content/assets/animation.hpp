#pragma once

#include "content/asset.hpp"
#include "libold/content/loaders/animation_loader.hpp"
#include "libold/content/flags/key_flag.hpp"
#include "libold/content/flags/key_marker_type.hpp"
#include "utility/flag_set.hpp"
#include "animation_node.hpp"
#include <vector>

namespace gorc {

class content_manager;

namespace content {
namespace assets {

class animation : public asset {
public:
    static fourcc const type;

    flag_set<flags::key_flag> flags;
    unsigned int frame_count;
    double framerate;

    std::vector<std::tuple<double, flags::key_marker_type>> markers;
    std::vector<animation_node> nodes;
};

}
}

asset_ref<content::assets::animation> get_asset(content_manager &cm, keyframe_id);

}
