#pragma once

#include <unordered_map>
#include <memory>
#include <boost/filesystem/path.hpp>

#include "loader.hpp"
#include "content/asset.hpp"
#include "vfs/virtual_file_system.hpp"
#include "utility/service_registry.hpp"
#include "loader_registry.hpp"

namespace gorc {
namespace content {

class content_manager {
private:
    std::vector<std::unique_ptr<asset>> assets;
    std::unordered_map<std::string, int> asset_map;
    service_registry const &services;
    const virtual_file_system& fs;
    loader_registry const &loaders;

    std::tuple<int, asset*> internal_load(const boost::filesystem::path& name, const std::vector<boost::filesystem::path>& basepaths, loader const &loader);

public:
    content_manager(service_registry const &services);

    template <typename T> asset_ref<T> load(const boost::filesystem::path& name) {
        auto it = asset_map.find(name.generic_string());
        if(it != asset_map.end()) {
            return asset_ref<T>(reinterpret_cast<T const &>(*assets[it->second].get()),
                                asset_id(it->second));
        }

        // Load asset from scratch.
        auto const &loader = loaders.get_loader(T::type);
        auto val = internal_load(name, loader.get_prefixes(), loader);
        return asset_ref<T>(reinterpret_cast<T const &>(*std::get<1>(val)),
                            asset_id(std::get<0>(val)));
    }

    template <typename T> int load_id(const boost::filesystem::path& name) {
        auto it = asset_map.find(name.generic_string());
        if(it != asset_map.end()) {
            return it->second;
        }

        // Load asset from scratch.
        auto const &loader = loaders.get_loader(T::type);
        return std::get<0>(internal_load(name, loader.get_prefixes(), loader));
    }

    template <typename T> asset_ref<T> get_asset(int id) {
        return asset_ref<T>(reinterpret_cast<T&>(*assets[id].get()),
                            asset_id(id));
    }
};

}
}
