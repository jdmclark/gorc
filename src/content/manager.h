#pragma once

#include <unordered_map>
#include <memory>
#include <boost/filesystem/path.hpp>

#include "loader.h"
#include "asset.h"
#include "filesystem.h"
#include "framework/diagnostics/report.h"
#include "framework/io/exception.h"

namespace Gorc {
namespace Content {

class Manager {
private:
	std::vector<std::unique_ptr<Asset>> assets;
	std::unordered_map<std::string, int> asset_map;
	Diagnostics::Report& report;
	const FileSystem& fs;

	std::tuple<int, Asset*> InternalLoad(const boost::filesystem::path& name, const std::vector<boost::filesystem::path>& basepaths, Loader& loader);

public:
	Manager(Diagnostics::Report& report, const FileSystem& fs);

	template <typename T, typename... ArgT> const T& Load(const boost::filesystem::path& name, ArgT... args) {
		auto it = asset_map.find(name.generic_string());
		if(it != asset_map.end()) {
			return *reinterpret_cast<T*>(assets[it->second].get());
		}

		// Load asset from scratch.
		typename T::Loader loader(args...);
		return *reinterpret_cast<T*>(std::get<1>(InternalLoad(name, loader.AssetRootPath, loader)));
	}

	template <typename T, typename... ArgT> int LoadId(const boost::filesystem::path& name, ArgT... args) {
		auto it = asset_map.find(name.generic_string());
		if(it != asset_map.end()) {
			return it->second;
		}

		// Load asset from scratch.
		typename T::Loader loader(args...);
		return std::get<0>(InternalLoad(name, loader.AssetRootPath, loader));
	}

	template <typename T> const T& GetAsset(int asset_id) {
		return *reinterpret_cast<T*>(assets[asset_id].get());
	}
};

}
}
