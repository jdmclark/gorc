#pragma once

#include "content/asset.h"
#include "content/loaders/script_loader.h"

namespace Gorc {
namespace Content {
namespace Assets {

class Script : public Asset {
public:
	using Loader = Loaders::ScriptLoader;

	Cog::Script Script;
};

}
}
}
