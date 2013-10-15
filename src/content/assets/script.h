#pragma once

#include "content/asset.h"
#include "content/loaders/script_loader.h"
#include "cog/script.h"
#include "cog/instance.h"
#include "framework/pool.h"
#include <unordered_map>
#include <string>

namespace gorc {
namespace content {

class manager;

namespace assets {

class colormap;
class level_adjoin;
class level_sector;
class level_surface;

class script : public asset {
public:
	using loader = loaders::script_loader;

	cog::script script;
};

}
}
}
