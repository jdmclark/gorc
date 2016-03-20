#pragma once

#include "content/loader.hpp"

namespace gorc {
namespace content {
namespace loaders {

class shader_program_loader : public loader {
public:
    virtual std::vector<path> const& get_prefixes() const override;
};

class vertex_program_loader : public shader_program_loader {
public:
    static fourcc const type;

    virtual std::unique_ptr<asset> deserialize(input_stream &file,
                                               content_manager &,
                                               service_registry const &) const override;

};

class fragment_program_loader : public shader_program_loader {
public:
    static fourcc const type;

    virtual std::unique_ptr<asset> deserialize(input_stream &file,
                                               content_manager &,
                                               service_registry const &) const override;

};

}
}
}
