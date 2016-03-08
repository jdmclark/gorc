#pragma once

#include "libold/base/content/loader.hpp"
#include "math/vector.hpp"

namespace gorc {
namespace content {

namespace assets {
class colormap;
class bitmap;
}

namespace loaders {

class bitmap_loader : public loader {
private:
    assets::colormap const* pal_colormap = nullptr;
    assets::bitmap const* pal_bitmap = nullptr;
    bool disable_transparency;

public:
    static const std::vector<boost::filesystem::path> asset_root_path;

    bitmap_loader(const assets::colormap& colormap, bool disable_transparency = false);
    bitmap_loader(const assets::bitmap& bitmap, bool disable_transparency = false);

    virtual std::unique_ptr<asset> deserialize(input_stream &file, content_manager &, service_registry const &) const override;
};

}
}
}
