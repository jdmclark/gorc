#include "inventory_loader.h"
#include "content/assets/sprite.h"
#include "framework/diagnostics/helper.h"
#include "framework/io/exception.h"
#include "content/manager.h"
#include <boost/format.hpp>

const std::vector<boost::filesystem::path> Gorc::Content::Loaders::SpriteLoader::AssetRootPath = { "misc/spr" };

Gorc::Content::Loaders::SpriteLoader::SpriteLoader(const Content::Assets::Colormap& colormap) : colormap(colormap) {
	return;
}

std::unique_ptr<Gorc::Content::Asset> Gorc::Content::Loaders::SpriteLoader::Parse(Text::Tokenizer& t,
		Content::Manager& manager, Diagnostics::Report& report) {
	std::unique_ptr<Content::Assets::Sprite> spr(new Content::Assets::Sprite());

	spr->Material = &manager.Load<Assets::Material>(t.GetSpaceDelimitedString(), colormap);
	spr->Type = t.GetNumber<int>();
	spr->Width = t.GetNumber<float>();
	spr->Height = t.GetNumber<float>();
	spr->GeometryMode = static_cast<Flags::GeometryMode>(t.GetNumber<uint32_t>());
	spr->LightMode = static_cast<Flags::LightMode>(t.GetNumber<uint32_t>());
	spr->TextureMode = static_cast<Flags::TextureMode>(t.GetNumber<uint32_t>());
	spr->ExtraLight = t.GetNumber<float>();
	Math::Get<0>(spr->Offset) = t.GetNumber<float>();
	Math::Get<1>(spr->Offset) = t.GetNumber<float>();
	Math::Get<2>(spr->Offset) = t.GetNumber<float>();

	return std::unique_ptr<Asset>(std::move(spr));
}
