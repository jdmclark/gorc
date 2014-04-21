#pragma once

#include <unordered_map>
#include <memory>
#include <boost/filesystem/path.hpp>

#include "loader.h"
#include "asset.h"
#include "base/io/file_system.h"
#include "framework/diagnostics/report.h"
#include "base/io/exception.h"

namespace gorc {
namespace content {

class content_manager {
private:
    std::vector<std::unique_ptr<asset>> assets;
    std::unordered_map<std::string, int> asset_map;
    diagnostics::report& report;
    const io::file_system& fs;

    std::tuple<int, asset*> internal_load(const boost::filesystem::path& name, const std::vector<boost::filesystem::path>& basepaths, loader& loader);

public:
    content_manager(diagnostics::report& report, const io::file_system& fs);

    template <typename T, typename... ArgT> const T& load(const boost::filesystem::path& name, ArgT... args) {
        auto it = asset_map.find(name.generic_string());
        if(it != asset_map.end()) {
            return *reinterpret_cast<T*>(assets[it->second].get());
        }

        // Load asset from scratch.
        typename T::loader loader(args...);
        return *reinterpret_cast<T*>(std::get<1>(internal_load(name, loader.asset_root_path, loader)));
    }

    template <typename T, typename... ArgT> int load_id(const boost::filesystem::path& name, ArgT... args) {
        auto it = asset_map.find(name.generic_string());
        if(it != asset_map.end()) {
            return it->second;
        }

        // Load asset from scratch.
        typename T::loader loader(args...);
        return std::get<0>(internal_load(name, loader.asset_root_path, loader));
    }

    template <typename T> const T& get_asset(int asset_id) {
        return *reinterpret_cast<T*>(assets[asset_id].get());
    }
};

}
}
