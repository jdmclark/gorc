#include "compiler_properties.hpp"
#include "source_file_entity.hpp"
#include "parse_source_file.hpp"
#include "build/boc-build/engine/entity_allocator.hpp"
#include <unordered_set>
#include <boost/filesystem.hpp>

gorc::compiler_properties::compiler_properties(compiler_configuration const &config)
    : config(config)
{
    return;
}

gorc::compiler_properties::~compiler_properties()
{
    return;
}

bool gorc::compiler_properties::create_dependencies(source_file_entity *sf,
                                                    service_registry const &services)
{
    LOG_INFO(format("Updating '%s' header dependencies") % sf->name());

    entity_allocator &ea = services.get<entity_allocator>();
    std::unordered_set<source_file_entity*> remaining = { sf };
    bool rv = true;

    while(!remaining.empty()) {
        auto it = remaining.begin();
        source_file_entity *next = *it;
        remaining.erase(it);

        auto this_deps = get_source_file_headers(next->file_path(), config.header_search_paths);
        if(!this_deps.has_value()) {
            rv = false;
            continue;
        }

        std::unordered_set<entity*> new_deps;
        for(auto const &dep : this_deps.get_value()) {
            source_file_entity *header_ent;
            bool owned_by_this_thread;
            std::tie(header_ent, owned_by_this_thread) =
                ea.maybe_emplace_file<source_file_entity>(
                        dep,
                        boost::filesystem::last_write_time(dep));

            new_deps.insert(header_ent);

            if(owned_by_this_thread) {
                remaining.insert(header_ent);
            }
        }

        next->set_header_dependencies(new_deps);
    }

    return rv;
}
