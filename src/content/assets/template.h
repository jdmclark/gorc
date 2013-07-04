#pragma once

#include "framework/math/vector.h"
#include "framework/flagset.h"
#include "framework/text/tokenizer.h"
#include "flags.h"
#include "model.h"
#include "colormap.h"
#include <string>
#include <memory>

namespace Gorc {
namespace Content {

class Manager;

namespace Assets {

class Template {
public:
	// General fields
	unsigned int Sector;
	Math::Vector<3> Position;
	Math::Vector<3> Orientation;

	Model const* Model3d = nullptr;
	ThingType Type = ThingType::Ghost;
	MoveType Move = MoveType::None;
	CollideType Collide = CollideType::None;

	float Mass = 2.0f;
	float Radius = 0.05f;

	void ParseArgs(Text::Tokenizer& tok, Manager& manager, const Colormap& cmp, Diagnostics::Report& report);
};

}
}
}
