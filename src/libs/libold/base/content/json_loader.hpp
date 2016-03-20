#pragma once

#include "content/loader.hpp"
#include "text/json_input_stream.hpp"

namespace gorc {
namespace content {

class json_loader : public loader {
protected:
    virtual std::unique_ptr<asset> parse(json_input_stream &is,
                                         content_manager &,
                                         service_registry const &) const = 0;

public:
    virtual std::unique_ptr<asset> deserialize(input_stream &file,
                                               content_manager &,
                                               service_registry const &) const override;
};

}
}
