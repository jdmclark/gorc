#pragma once

#include "model_face.h"
#include <memory>
#include <vector>

namespace gorc {
namespace content {
namespace assets {

class model_mesh {
public:
	int index;
	std::string name;
	float radius;
	flags::geometry_mode geo;
	flags::light_mode light;
	flags::texture_mode tex;

	std::vector<vector<3>> vertices;
	std::vector<vector<3>> vertex_normals;
	std::vector<vector<2>> texture_vertices;

	std::vector<model_face> faces;

	std::vector<int> mesh_index_buffer;
};

}
}
}
