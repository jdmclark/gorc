#pragma once

#include "framework/math/vector.h"
#include "framework/flagset.h"
#include "framework/text/tokenizer.h"
#include "cog/compiler.h"
#include "flags.h"
#include "model.h"
#include "soundclass.h"
#include "colormap.h"
#include "script.h"
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
	SoundClass const* SoundClass = nullptr;
	Script const* Cog = nullptr;

	ThingType Type = ThingType::Ghost;
	MoveType Move = MoveType::None;
	CollideType Collide = CollideType::None;
	FlagSet<ThingFlag> Flags;
	Math::Vector<3> EyeOffset;

	float Health = 100.0f;
	float MaxHealth = 100.0f;

	std::vector<std::tuple<Math::Vector<3>, Math::Vector<3>>> Frames;

	float Mass = 2.0f;
	float Size = 0.05f;
	float MoveSize = 0.05f;
	float Height = 0.18f;

	void ParseArgs(Text::Tokenizer& tok, Manager& manager, const Colormap& cmp, const Cog::Compiler& compiler, Diagnostics::Report& report);
};

}
}
}
