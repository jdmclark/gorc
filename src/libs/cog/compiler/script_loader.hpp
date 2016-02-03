#pragma once

#include "cog/script/script.hpp"
#include "content/loader.hpp"

namespace gorc {
    namespace cog {

        class script_loader : public loader {
        public:
            virtual std::unique_ptr<asset> deserialize(input_stream &is,
                                                       content_manager &,
                                                       service_registry const &) const override;
        };

    }
}
