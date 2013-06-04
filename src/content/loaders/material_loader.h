#pragma once

#include "content/loader.h"
#include "content/assets/colormap.h"

namespace Gorc {
namespace Content {
namespace Loaders {

class MaterialLoader : public Loader {
protected:
	const Assets::Colormap& Colormap;

public:
	MaterialLoader(const Assets::Colormap& Colormap);

	virtual std::unique_ptr<Asset> Deserialize(IO::ReadOnlyFile& file, Manager& manager, Diagnostics::Report& report) override;
};

}
}
}
