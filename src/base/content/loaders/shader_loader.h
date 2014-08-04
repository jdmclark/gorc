#pragma once

#include "base/content/json_loader.h"

namespace gorc {
namespace content {
namespace loaders {

class shader_loader : public json_loader {
public:
    static const std::vector<boost::filesystem::path> asset_root_path;

    virtual std::unique_ptr<asset> parse(boost::filesystem::path const &filename,
                                         io::json_input_stream &file,
                                         content_manager &manager,
                                         diagnostics::report &report) override;
};

}
}
}
