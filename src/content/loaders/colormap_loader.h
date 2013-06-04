#pragma once

#include "content/loader.h"

namespace Gorc {
namespace Content {
namespace Loaders {

class ColormapLoader : public Loader {
public:
	virtual std::unique_ptr<Asset> Deserialize(IO::ReadOnlyFile& file, Manager& manager, Diagnostics::Report& report) override;
};

}
}
}
