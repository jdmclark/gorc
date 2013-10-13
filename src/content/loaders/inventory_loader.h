#pragma once

#include "content/text_loader.h"
#include "cog/compiler.h"

namespace Gorc {
namespace Content {
namespace Loaders {

class InventoryLoader : public TextLoader {
private:
	Cog::Compiler& compiler;

public:
	static const std::vector<boost::filesystem::path> AssetRootPath;

	InventoryLoader(Cog::Compiler& compiler);

	virtual std::unique_ptr<Asset> Parse(Text::Tokenizer& t, Manager& manager, Diagnostics::Report& report) override;
};

}
}
}
