#pragma once

#include "framework/content/text_loader.h"
#include "content/assets/colormap.h"

namespace gorc {
namespace content {
namespace loaders {

class sprite_loader : public text_loader {
private:
    const content::assets::colormap& colormap;

public:
    static const std::vector<boost::filesystem::path> asset_root_path;

    sprite_loader(const content::assets::colormap& colormap);

    virtual std::unique_ptr<asset> parse(text::tokenizer& t, manager& manager, diagnostics::report& report) override;
};

}
}
}
