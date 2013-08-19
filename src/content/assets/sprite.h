#pragma once

#include "content/asset.h"
#include "content/loaders/sprite_loader.h"
#include "content/flags/geometrymode.h"
#include "content/flags/lightmode.h"
#include "content/flags/texturemode.h"
#include "framework/math/vector.h"

#include "material.h"

namespace Gorc {
namespace Content {
namespace Assets {

class Sprite : public Asset {
public:
	using Loader = Loaders::SpriteLoader;

	const Material* Material;
	int Type;
	float Width;
	float Height;
	Flags::GeometryMode GeometryMode;
	Flags::LightMode LightMode;
	Flags::TextureMode TextureMode;
	float ExtraLight;
	Math::Vector<3> Offset;
};

}
}
}
