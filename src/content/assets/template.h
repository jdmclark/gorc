#pragma once

#include "framework/math/vector.h"
#include "framework/flagset.h"
#include "framework/text/tokenizer.h"
#include "framework/flagset.h"
#include "cog/compiler.h"
#include "content/flags/thingtype.h"
#include "content/flags/movetype.h"
#include "content/flags/collidetype.h"
#include "content/flags/thingflag.h"
#include "content/flags/attachflag.h"
#include "content/flags/actorflag.h"
#include "content/flags/physicsflag.h"
#include "model.h"
#include "soundclass.h"
#include "colormap.h"
#include "script.h"
#include "puppet.h"
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
	Math::Vector<3> Thrust;

	Model const* Model3d = nullptr;
	SoundClass const* SoundClass = nullptr;
	Script const* Cog = nullptr;
	Puppet const* Puppet = nullptr;

	Flags::ThingType Type = Flags::ThingType::Ghost;
	Flags::MoveType Move = Flags::MoveType::None;
	Flags::CollideType Collide = Flags::CollideType::None;
	FlagSet<Flags::ThingFlag> Flags;
	FlagSet<Flags::AttachFlag> AttachFlags;
	FlagSet<Flags::PhysicsFlag> PhysicsFlags;
	Math::Vector<3> EyeOffset;

	FlagSet<Flags::ActorFlag> ActorFlags;

	float Health = 100.0f;
	float MaxHealth = 100.0f;

	std::vector<std::tuple<Math::Vector<3>, Math::Vector<3>>> Frames;

	float Mass = 2.0f;
	float Size = 0.05f;
	float MoveSize = 0.05f;
	float Height = 0.18f;

	float Light = 0.0f;
	float MaxLight = 1.0f;
	float LightIntensity = 0.0f;
	Math::Vector<3> LightOffset;

	void ParseArgs(Text::Tokenizer& tok, Manager& manager, const Colormap& cmp, const Cog::Compiler& compiler, Diagnostics::Report& report);
};

}
}
}
