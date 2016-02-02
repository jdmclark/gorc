#include "content_manager.hpp"
#include "loader_registry.hpp"
#include "vfs/virtual_file_system.hpp"
#include <algorithm>

namespace {

    std::string canonical_content_name(std::string const &name)
    {
        std::string rv;
        rv.reserve(name.size());
        std::transform(name.begin(), name.end(), std::back_inserter(rv), tolower);
        return rv;
    }

}

gorc::content_manager::asset_data::asset_data(fourcc type,
                                              std::string const &name)
    : type(type)
    , name(name)
{
    return;
}

size_t gorc::content_manager::load_internal(fourcc type, std::string const &name)
{
    std::string real_name = canonical_content_name(name);

    auto loaded_it = asset_map.find(real_name);
    if(loaded_it == asset_map.end()) {
        size_t new_element = assets.size();
        assets.emplace_back(type, real_name);
        loaded_it = asset_map.emplace(real_name, new_element).first;
    }

    // Chunk may need to be finalized
    auto &element = assets[loaded_it->second];
    if(!element.content) {
        auto const &loader = services.get<loader_registry>().get_loader(element.type);
        auto file = services.get<virtual_file_system>().open(real_name);
        element.content = loader.deserialize(*file, *this, services);
    }

    return loaded_it->second;
}

gorc::content_manager::content_manager(service_registry const &services)
    : services(services)
{
    return;
}
