#pragma once

#include "content/asset.h"
#include "content/loaders/model_loader.h"

#include "model_geoset.h"
#include "model_node.h"
#include "material.h"

#include <memory>

namespace Gorc {
namespace Content {
namespace Assets {

class Model : public Asset {
public:
	using Loader = Loaders::ModelLoader;

	std::vector<std::string> MaterialEntries;
	std::vector<Material const*> Materials;

	float Radius;
	Math::Vector<3> InsertOffset;

	std::vector<ModelGeoSet> GeoSets;
	std::vector<ModelNode> HierarchyNodes;
};

}
}
}
