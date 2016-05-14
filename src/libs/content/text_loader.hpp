#pragma once

#include "content/loader.hpp"
#include "text/generic_tokenizer.hpp"

namespace gorc {

    class text_loader : public loader {
    protected:
        virtual std::unique_ptr<asset> parse(generic_tokenizer &t,
                                             content_manager &manager,
                                             service_registry const &) const = 0;

    public:
        virtual std::unique_ptr<asset> deserialize(input_stream &,
                                                   content_manager &,
                                                   asset_id,
                                                   service_registry const &) const override;
    };

}
