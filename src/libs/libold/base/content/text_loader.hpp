#pragma once

#include "content/loader.hpp"
#include "libold/base/text/tokenizer.hpp"

namespace gorc {
    namespace content {

        class text_loader : public loader {
        protected:
            virtual std::unique_ptr<asset> parse(text::tokenizer &t,
                                                 content_manager &manager,
                                                 service_registry const &) const = 0;

        public:
            virtual std::unique_ptr<asset> deserialize(input_stream &,
                                                       content_manager &,
                                                       asset_id,
                                                       service_registry const &,
                                                       std::string const &name) const override;
        };
    }
}
