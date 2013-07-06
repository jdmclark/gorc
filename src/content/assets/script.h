#pragma once

#include "content/asset.h"
#include "content/loaders/script_loader.h"
#include "cog/script.h"
#include "cog/instance.h"
#include <unordered_map>
#include <string>

namespace Gorc {
namespace Content {

class Manager;

namespace Assets {

class Colormap;

class Script : public Asset {
private:
	void LoadInstanceResources(Cog::Instance& inst, Manager& manager, Cog::Compiler& compiler, const Assets::Colormap& colormap,
			const std::unordered_map<std::string, int>& template_map) const;

public:
	using Loader = Loaders::ScriptLoader;

	Cog::Script Script;

	std::unique_ptr<Cog::Instance> CreateInstance(Manager& manager, Cog::Compiler& compiler, const Assets::Colormap& colormap,
			const std::unordered_map<std::string, int>& template_map) const;
	std::unique_ptr<Cog::Instance> CreateInstance(Manager& manager, Cog::Compiler& compiler, const Assets::Colormap& colormap,
			const std::unordered_map<std::string, int>& template_map, const std::vector<Cog::VM::Value>& values) const;
};

}
}
}
