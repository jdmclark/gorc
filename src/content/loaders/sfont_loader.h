#pragma once

#include "base/content/loader.h"

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
    bool disable_transparency;

public:
    static const std::vector<boost::filesystem::path> asset_root_path;

    sfont_loader(const assets::colormap& colormap,
                 bool disable_transparency = false);
    sfont_loader(const assets::bitmap& bitmap,
                 bool disable_transparency = false);

    virtual std::unique_ptr<asset> deserialize(const boost::filesystem::path& filename,
            io::read_only_file& file, content_manager& manager, diagnostics::report& report) override;
};

}
}
}
