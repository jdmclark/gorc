#pragma once

#include "entities/root_entity.hpp"
#include "engine/entity_allocator.hpp"
#include "project_file.hpp"

namespace gorc {

    root_entity* create_graph_nodes(project_file const &pf,
                                    entity_allocator &ea);

}
