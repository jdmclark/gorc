#pragma once

#include "framework/content/text_loader.h"
#include "cog/compiler.h"

namespace gorc {
namespace content {
namespace loaders {

class puppet_loader : public text_loader {
public:
    static const std::vector<boost::filesystem::path> asset_root_path;

    virtual std::unique_ptr<asset> parse(text::tokenizer& t, content_manager& manager, diagnostics::report& report) override;
};

}
}
}
