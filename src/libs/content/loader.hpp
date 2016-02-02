#pragma once

#include <memory>
#include "asset.hpp"

namespace gorc {

    class input_stream;
    class content_manager;
    class service_registry;

    class loader {
    public:
        virtual ~loader();

        virtual std::unique_ptr<asset> deserialize(input_stream &,
                                                   content_manager &,
                                                   service_registry const &) const = 0;
    };

}
