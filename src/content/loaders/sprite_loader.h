#pragma once

#include "content/text_loader.h"
#include "content/assets/colormap.h"

namespace Gorc {
namespace Content {
namespace Loaders {

class SpriteLoader : public TextLoader {
private:
	const Content::Assets::Colormap& colormap;

public:
	static const std::vector<boost::filesystem::path> AssetRootPath;

	SpriteLoader(const Content::Assets::Colormap& colormap);

	virtual std::unique_ptr<Asset> Parse(Text::Tokenizer& t, Manager& manager, Diagnostics::Report& report) override;
};

}
}
}
