#pragma once

#include "content/loader.h"

namespace Gorc {
namespace Content {
namespace Loaders {

class ShaderLoader : public Loader {
public:
	static const std::vector<boost::filesystem::path> AssetRootPath;

	virtual std::unique_ptr<Asset> Deserialize(IO::ReadOnlyFile& file, Manager& manager, Diagnostics::Report& report) override;
};

}
}
}
