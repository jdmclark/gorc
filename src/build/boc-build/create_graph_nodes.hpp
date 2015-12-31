#pragma once

#include "entities/root_entity.hpp"
#include "engine/entity_allocator.hpp"
#include "utility/service_registry.hpp"
#include "project_file.hpp"

namespace gorc {

    root_entity* create_graph_nodes(project_file const &pf,
                                    service_registry const &services,
                                    entity_allocator &ea);

}
