#pragma once

#include "framework/content/asset.h"
#include "content/loaders/animation_loader.h"
#include "content/flags/key_flag.h"
#include "content/flags/key_marker_type.h"
#include "framework/utility/flag_set.h"
#include "animation_node.h"
#include <vector>

namespace gorc {
namespace content {
namespace assets {

class animation : public asset {
public:
	using loader = loaders::animation_loader;

	flag_set<flags::key_flag> flags;
	unsigned int frame_count;
	double framerate;

	std::vector<std::tuple<double, flags::key_marker_type>> markers;
	std::vector<animation_node> nodes;
};

}
}
}
