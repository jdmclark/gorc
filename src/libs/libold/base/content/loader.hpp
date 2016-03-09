#pragma once

#include "io/input_stream.hpp"
#include "content/fourcc.hpp"
#include "content/asset.hpp"
#include <memory>
#include <boost/filesystem/path.hpp>

namespace gorc {
namespace content {

class content_manager;

class loader {
public:
    virtual ~loader();

    virtual std::unique_ptr<asset> deserialize(input_stream &,
                                               content_manager &,
                                               service_registry const &) const = 0;
};

}
}
