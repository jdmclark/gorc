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
#include "sprite.h"
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

	FlagSet<Flags::ActorFlag> ActorFlags;
	FlagSet<Flags::AttachFlag> AttachFlags;
	Script const* Cog = nullptr;
	Flags::CollideType Collide = Flags::CollideType::None;
	int CreateThing = 0;
	int Explode = 0;
	Math::Vector<3> EyeOffset;
	FlagSet<Flags::ThingFlag> Flags;
	int FleshHit = 0;
	std::vector<std::tuple<Math::Vector<3>, Math::Vector<3>>> Frames;
	float Health = 100.0f;
	float Height = 0.18f;
	float Light = 0.0f;
	float LightIntensity = 0.0f;
	Math::Vector<3> LightOffset;
	float Mass = 2.0f;
	float MaxHealth = 100.0f;
	float MaxLight = 1.0f;
	Model const* Model3d = nullptr;
	Flags::MoveType Move = Flags::MoveType::None;
	float MoveSize = 0.05f;
	FlagSet<Flags::PhysicsFlag> PhysicsFlags;
	Puppet const* Puppet = nullptr;
	float Size = 0.05f;
	SoundClass const* SoundClass = nullptr;
	Sprite const* Sprite = nullptr;
	float Timer = 0.0f;
	Flags::ThingType Type = Flags::ThingType::Ghost;

	void ParseArgs(Text::Tokenizer& tok, Manager& manager, const Colormap& cmp, const Cog::Compiler& compiler,
			const std::unordered_map<std::string, int>& templates, Diagnostics::Report& report);
};

}
}
}
