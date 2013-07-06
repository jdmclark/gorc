#pragma once

#include "content/asset.h"
#include "content/loaders/script_loader.h"
#include "cog/script.h"
#include "cog/instance.h"
#include "framework/pool.h"
#include <unordered_map>
#include <string>

namespace Gorc {
namespace Content {

class Manager;

namespace Assets {

class Colormap;
class LevelAdjoin;
class LevelSector;
class LevelSurface;

class Script : public Asset {
public:
	using Loader = Loaders::ScriptLoader;

	Cog::Script Script;
};

}
}
}
