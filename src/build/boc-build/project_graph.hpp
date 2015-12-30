#pragma once

#include "engine/entity_allocator.hpp"
#include "engine/entity_registry.hpp"
#include "entities/root_entity.hpp"
#include "io/path.hpp"

namespace gorc {

    class project_graph {
    private:
        root_entity *root = nullptr;
        entity_registry const &reg;
        entity_allocator ea;
        path cache_filename;

    public:
        project_graph(entity_registry const &reg,
                      path const &project_filename,
                      path const &cache_filename);

        ~project_graph();

        root_entity* get_root();
    };

}
