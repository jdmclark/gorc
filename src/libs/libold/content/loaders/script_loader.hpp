#pragma once

#include "libold/base/content/loader.hpp"
#include "libold/cog/compiler.hpp"

namespace gorc {
namespace content {
namespace loaders {

class script_loader : public loader {
public:
    static const std::vector<boost::filesystem::path> asset_root_path;

    virtual std::unique_ptr<asset> deserialize(
            input_stream& file, content_manager& manager, service_registry const &) const override;
};

}
}
}
