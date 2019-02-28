#pragma once

#include "asset.hpp"
#include "io/path.hpp"
#include <memory>

namespace gorc {

    class input_stream;
    class content_manager;
    class service_registry;

    class loader {
    public:
        virtual ~loader();

        virtual std::unique_ptr<asset> deserialize(input_stream &,
                                                   content_manager &,
                                                   asset_id,
                                                   service_registry const &,
                                                   std::string const &name) const = 0;

        virtual std::vector<path> const &get_prefixes() const = 0;
        virtual maybe<char const *> get_default() const;
    };
}
