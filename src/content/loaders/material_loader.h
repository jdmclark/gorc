#pragma once

#include "base/content/loader.h"
#include "content/assets/colormap.h"

namespace gorc {
namespace content {
namespace loaders {

class material_loader : public loader {
protected:
    const assets::colormap& colormap;

public:
    static const std::vector<boost::filesystem::path> asset_root_path;

    material_loader(const assets::colormap& colormap);

    virtual std::unique_ptr<asset> deserialize(const boost::filesystem::path& filename,
            io::read_only_file& file, content_manager& manager, diagnostics::report& report) override;
};

}
}
}
