#pragma once

#include "content/text_loader.hpp"

namespace gorc {

    class inventory_loader : public text_loader {
    public:
        static fourcc const type;

        virtual std::unique_ptr<asset> parse(generic_tokenizer &t,
                                             content_manager &,
                                             service_registry const &) const override;

        virtual std::vector<path> const& get_prefixes() const override;
    };

}
