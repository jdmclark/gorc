#pragma once

#include "model_face.h"
#include <btBulletDynamicsCommon.h>
#include <memory>
#include <vector>

namespace Gorc {
namespace Content {
namespace Assets {

class ModelMesh {
public:
	unsigned int Index;
	std::string Name;
	float Radius;
	GeometryMode Geo;
	LightMode Light;
	TextureMode Tex;

	std::vector<Math::Vector<3>> Vertices;
	std::vector<Math::Vector<3>> VertexNormals;
	std::vector<Math::Vector<2>> TextureVertices;

	std::vector<ModelFace> Faces;

	std::vector<int> MeshIndexBuffer;
	std::unique_ptr<btCompoundShape> Shape;
};

}
}
}
