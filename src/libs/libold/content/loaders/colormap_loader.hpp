#pragma once

#include "libold/base/content/loader.hpp"

namespace gorc {
namespace content {
namespace loaders {

class colormap_loader : public loader {
public:
    static const std::vector<boost::filesystem::path> asset_root_path;

    virtual std::unique_ptr<asset> deserialize(input_stream &,
                                               content_manager &,
                                               service_registry const &) const override;
};

}
}
}
