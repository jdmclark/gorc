#pragma once

#include "content/asset.h"
#include "content/loaders/level_loader.h"

#include "framework/math/vector.h"
#include "level_header.h"
#include "level_adjoin.h"
#include "level_surface.h"
#include "level_sector.h"

namespace Gorc {
namespace Content {
namespace Assets {

class Level : public Asset {
public:
	inline static Loaders::LevelLoader GetLoader() {
		return Loaders::LevelLoader();
	}

	LevelHeader Header;
	std::vector<std::tuple<std::string, float, float>> MaterialEntries;
	std::vector<std::string> ColormapEntries;

	std::vector<Math::Vector<3>> Vertices;
	std::vector<Math::Vector<2>> TextureVertices;
	std::vector<LevelAdjoin> Adjoins;
	std::vector<LevelSurface> Surfaces;
	std::vector<LevelSector> Sectors;
};

}
}
}
