#pragma once

#include "engine/entity_allocator.hpp"
#include "engine/entity_registry.hpp"
#include "entities/root_entity.hpp"
#include "utility/service_registry.hpp"
#include "io/path.hpp"

namespace gorc {

    class project_graph {
    private:
        root_entity *root = nullptr;
        entity_registry const &reg;
        path cache_filename;

    public:
        project_graph(service_registry const &services,
                      entity_registry const &reg,
                      entity_allocator &ea,
                      path const &project_filename,
                      path const &cache_filename);

        ~project_graph();

        root_entity* get_root();
    };

}
