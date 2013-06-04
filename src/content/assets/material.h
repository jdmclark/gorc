#pragma once

#include "content/loaders/material_loader.h"
#include "material_cel.h"
#include <vector>

namespace Gorc {
namespace Content {
namespace Assets {

class Material : public Asset {
public:
	using Loader = Loaders::MaterialLoader;

	unsigned int Width, Height;
	std::vector<MaterialCel> Cels;
};

}
}
}
