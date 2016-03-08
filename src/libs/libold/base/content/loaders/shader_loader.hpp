#pragma once

#include "libold/base/content/json_loader.hpp"

namespace gorc {
namespace content {
namespace loaders {

class shader_loader : public json_loader {
public:
    static const std::vector<boost::filesystem::path> asset_root_path;

    virtual std::unique_ptr<asset> parse(json_input_stream &file,
                                         content_manager &manager,
                                         service_registry const &) const override;
};

}
}
}
