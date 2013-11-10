#pragma once

#include "content/asset.h"
#include "content/loaders/model_loader.h"

#include "model_geoset.h"
#include "model_node.h"
#include "material.h"

#include <memory>

namespace gorc {
namespace content {
namespace assets {

class model : public asset {
public:
	using loader = loaders::model_loader;

	std::vector<std::string> material_entries;
	std::vector<material const*> materials;

	float radius;
	vector<3> insert_offset;

	std::vector<model_geoset> geosets;
	std::vector<model_node> hierarchy_nodes;
};

}
}
}
