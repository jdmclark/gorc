#pragma once

#include "content/asset.h"
#include "content/loaders/soundclass_loader.h"
#include "soundsubclass.h"
#include <array>

namespace Gorc {
namespace Content {
namespace Assets {

class SoundClass : public Asset {
public:
	using Loader = Loaders::SoundClassLoader;

	std::array<SoundSubclass, 128> Subclasses;
};

}
}
}
