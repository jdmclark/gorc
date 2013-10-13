#pragma once

#include "content/text_loader.h"
#include "content/assets/colormap.h"

namespace Gorc {
namespace Content {
namespace Loaders {

class ModelLoader : public TextLoader {
private:
	const Assets::Colormap& colormap;

public:
	static const std::vector<boost::filesystem::path> AssetRootPath;

	ModelLoader(const Assets::Colormap& colormap);

	virtual std::unique_ptr<Asset> Parse(Text::Tokenizer& t, Manager& manager, Diagnostics::Report& report) override;
};

}
}
}
