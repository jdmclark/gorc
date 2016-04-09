#pragma once

#include "content/loader.hpp"

namespace gorc {

    class colormap_loader : public loader {
    public:
        static fourcc const type;

        virtual std::unique_ptr<asset> deserialize(input_stream &is,
                                                   content_manager &,
                                                   service_registry const &) const override;
    };

}
