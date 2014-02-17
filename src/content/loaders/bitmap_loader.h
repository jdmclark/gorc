#pragma once

#include "framework/content/loader.h"
#include "framework/math/vector.h"

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

    vector<3, uint8_t> get_palette_color(uint8_t) const;

public:
    static const std::vector<boost::filesystem::path> asset_root_path;

    bitmap_loader(const assets::colormap& colormap);
    bitmap_loader(const assets::bitmap& bitmap);

    virtual std::unique_ptr<asset> deserialize(io::read_only_file& file, manager& manager, diagnostics::report& report) override;
};

}
}
}
