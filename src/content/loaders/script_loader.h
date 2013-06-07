#pragma once

#include "content/loader.h"
#include "cog/compiler.h"

namespace Gorc {
namespace Content {
namespace Loaders {

class ScriptLoader : public Loader {
	const Cog::Compiler& Compiler;

public:
	static const std::vector<boost::filesystem::path> AssetRootPath;

	ScriptLoader(const Cog::Compiler& Compiler);

	virtual std::unique_ptr<Asset> Deserialize(IO::ReadOnlyFile& file, Manager& manager, Diagnostics::Report& report) override;
};

}
}
}
