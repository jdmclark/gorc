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

    template <typename T> const T& load(const boost::filesystem::path& name) {
        auto it = asset_map.find(name.generic_string());
        if(it != asset_map.end()) {
            return *reinterpret_cast<T*>(assets[it->second].get());
        }

        // Load asset from scratch.
        auto const &loader = loaders.get_loader(T::type);
        return *reinterpret_cast<T*>(std::get<1>(internal_load(name, loader.get_prefixes(), loader)));
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

    template <typename T> const T& get_asset(int asset_id) {
        return *reinterpret_cast<T*>(assets[asset_id].get());
    }
};

}
}
