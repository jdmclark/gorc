#pragma once

#include "content/asset.h"
#include "content/loaders/sprite_loader.h"
#include "content/flags/geometry_mode.h"
#include "content/flags/light_mode.h"
#include "content/flags/texture_mode.h"
#include "framework/math/vector.h"

#include "material.h"

namespace gorc {
namespace content {
namespace assets {

class sprite : public asset {
public:
	using loader = loaders::sprite_loader;

	const material* material;
	int type;
	float width;
	float height;
	flags::geometry_mode geometry_mode;
	flags::light_mode light_mode;
	flags::texture_mode texture_mode;
	float extra_light;
	vector<3> offset;
};

}
}
}
