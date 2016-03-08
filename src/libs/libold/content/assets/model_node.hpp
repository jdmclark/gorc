#pragma once

#include "libold/content/flags/mesh_node_type.hpp"
#include "utility/flag_set.hpp"
#include "math/vector.hpp"
#include <string>

namespace gorc {
namespace content {
namespace assets {

class model_node {
public:
    flag_set<flags::mesh_node_type> type;
    int mesh;
    int parent;
    int child;
    int sibling;
    int num_children;

    vector<3> offset;
    vector<3> rotation;
    vector<3> pivot;

    std::string name;
};

}
}
}
