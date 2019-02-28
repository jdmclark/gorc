#pragma once

#include "content/loader.hpp"
#include "jk/cog/script/script.hpp"

namespace gorc {
    namespace cog {

        class script_loader : public loader {
        public:
            static fourcc const type;

            virtual std::vector<path> const &get_prefixes() const override;

            virtual std::unique_ptr<asset> deserialize(input_stream &is,
                                                       content_manager &,
                                                       asset_id,
                                                       service_registry const &,
                                                       std::string const &name) const override;
        };
    }
}
