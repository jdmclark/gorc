#pragma once

#include "content/loader.hpp"

namespace gorc {
namespace content {
namespace loaders {

class sound_loader : public loader {
public:
    static fourcc const type;

    virtual std::unique_ptr<asset> deserialize(input_stream &file,
                                               content_manager &,
                                               service_registry const &) const override;

    virtual std::vector<path> const& get_prefixes() const override;
};

}
}
}
