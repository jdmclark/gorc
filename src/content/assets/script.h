#pragma once

#include "content/asset.h"
#include "content/loaders/scriptloader.h"
#include "cog/compiler.h"

namespace Gorc {
namespace Content {
namespace Assets {

class Script : public Asset {
public:
	inline static Loaders::ScriptLoader GetLoader(const Cog::Compiler& compiler) {
		return Loaders::ScriptLoader(compiler);
	}

	Cog::Script Script;
};

}
}
}
