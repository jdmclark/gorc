#pragma once

#include "content/text_loader.h"
#include "cog/compiler.h"

namespace Gorc {
namespace Content {
namespace Loaders {

class PuppetLoader : public TextLoader {
public:
	static const std::vector<boost::filesystem::path> AssetRootPath;

	virtual std::unique_ptr<Asset> Parse(Text::Tokenizer& t, Manager& manager, Diagnostics::Report& report) override;
};

}
}
}
