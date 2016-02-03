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

gorc::content_manager::asset_data::asset_data(fourcc type, std::string const &name)
    : type(type)
    , name(name)
{
    return;
}

gorc::content_manager::asset_data::asset_data(deserialization_constructor_tag,
                                              binary_input_stream &bis)
    : type(binary_deserialize<fourcc>(bis))
    , name(binary_deserialize<std::string>(bis))
{
    return;
}

void gorc::content_manager::asset_data::binary_serialize_object(binary_output_stream &bos) const
{
    binary_serialize(bos, type);
    binary_serialize(bos, name);
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
    finalize_internal(loaded_it->second);
    return loaded_it->second;
}

void gorc::content_manager::finalize_internal(size_t id)
{
    auto &element = assets.at(id);
    if(!element.content) {
        auto const &loader = services.get<loader_registry>().get_loader(element.type);
        auto file = services.get<virtual_file_system>().open(element.name);
        element.content = loader.deserialize(*file, *this, services);
    }
}

gorc::asset const& gorc::content_manager::load_from_id(size_t id)
{
    finalize_internal(id);
    return *assets.at(id).content;
}

gorc::content_manager::content_manager(service_registry const &services)
    : services(services)
{
    return;
}

gorc::content_manager::content_manager(deserialization_constructor_tag, binary_input_stream &bis)
    : services(bis.services)
{
    binary_deserialize_range<asset_data>(bis, std::back_inserter(assets));
    return;
}

void gorc::content_manager::binary_serialize_object(binary_output_stream &bos) const
{
    binary_serialize_range(bos, assets);
}
