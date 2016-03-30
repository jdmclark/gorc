#pragma once

#include "libold/base/content/text_loader.hpp"

namespace gorc {
namespace content {
namespace loaders {

class inventory_loader : public text_loader {
public:
    static fourcc const type;

    virtual std::unique_ptr<asset> parse(text::tokenizer& t, content_manager& manager, service_registry const &) const override;

    virtual std::vector<path> const& get_prefixes() const override;
};

}
}
}
