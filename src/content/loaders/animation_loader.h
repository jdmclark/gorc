#pragma once

#include "content/text_loader.h"

namespace gorc {
namespace content {
namespace loaders {

class animation_loader : public text_loader {
public:
	static const std::vector<boost::filesystem::path> asset_root_path;

	virtual std::unique_ptr<asset> parse(text::tokenizer& t, manager& manager, diagnostics::report& report) override;
};

}
}
}
