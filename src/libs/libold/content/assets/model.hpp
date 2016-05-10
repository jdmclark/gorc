#pragma once

#include "content/asset.hpp"
#include "libold/content/loaders/model_loader.hpp"

#include "model_geoset.hpp"
#include "model_node.hpp"
#include "jk/content/material.hpp"

#include <memory>

namespace gorc {
namespace content {
namespace assets {

class model : public asset {
public:
    static fourcc const type;

    std::vector<std::string> material_entries;
    std::vector<asset_ref<material>> materials;

    float radius;
    vector<3> insert_offset;

    std::vector<model_geoset> geosets;
    std::vector<model_node> hierarchy_nodes;
};

}
}

template <>
struct id_asset_type<model_id> {
    using type = content::assets::model;
};

}
