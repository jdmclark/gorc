#pragma once

#include "framework/content/loader.h"

namespace gorc {
namespace content {

namespace assets {
class colormap;
class bitmap;
}

namespace loaders {

class sfont_loader : public loader {
private:
    assets::colormap const* pal_colormap = nullptr;
    assets::bitmap const* pal_bitmap = nullptr;

public:
    static const std::vector<boost::filesystem::path> asset_root_path;

    sfont_loader(const assets::colormap& colormap);
    sfont_loader(const assets::bitmap& bitmap);

    virtual std::unique_ptr<asset> deserialize(io::read_only_file& file, manager& manager, diagnostics::report& report) override;
};

}
}
}
