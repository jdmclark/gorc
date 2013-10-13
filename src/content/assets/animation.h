#pragma once

#include "content/asset.h"
#include "content/loaders/animation_loader.h"
#include "content/flags/key_flag.h"
#include "content/flags/key_marker_type.h"
#include "framework/flag_set.h"
#include "animation_node.h"
#include <vector>

namespace Gorc {
namespace Content {
namespace Assets {

class Animation : public Asset {
public:
	using Loader = Loaders::AnimationLoader;

	FlagSet<Flags::KeyFlag> Flags;
	unsigned int FrameCount;
	double FrameRate;

	std::vector<std::tuple<double, Flags::KeyMarkerType>> Markers;
	std::vector<AnimationNode> Nodes;
};

}
}
}
