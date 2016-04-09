#pragma once

#include "jk/cog/script/script.hpp"
#include "content/loader.hpp"

namespace gorc {
    namespace cog {

        class script_loader : public loader {
        public:
            static fourcc const type;

            virtual std::vector<path> const& get_prefixes() const override;

            virtual std::unique_ptr<asset> deserialize(input_stream &is,
                                                       content_manager &,
                                                       service_registry const &) const override;
        };

    }
}
