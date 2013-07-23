#pragma once

#include "flags.h"
#include "framework/flagset.h"
#include "framework/math/vector.h"
#include <string>

namespace Gorc {
namespace Content {
namespace Assets {

class ModelNode {
public:
	FlagSet<MeshNodeType> Type;
	int Mesh;
	int Parent;
	int Child;
	int Sibling;
	int NumChildren;

	Math::Vector<3> Offset;
	Math::Vector<3> Rotation;
	Math::Vector<3> Pivot;

	std::string Name;
};

}
}
}
