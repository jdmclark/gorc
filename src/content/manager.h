#pragma once

#include <unordered_map>
#include <memory>
#include <boost/filesystem/path.hpp>

#include "loader.h"
#include "asset.h"
#include "filesystem.h"
#include "framework/diagnostics/report.h"

namespace Gorc {
namespace Content {

class Manager {
private:
	std::unordered_map<std::string, std::unique_ptr<Asset>> assets;
	Diagnostics::Report& report;
	const FileSystem& fs;

	Asset* InternalLoad(const boost::filesystem::path& name, Loader& loader);

public:
	Manager(Diagnostics::Report& report, const FileSystem& fs);

	template <typename T, typename... ArgT> const T& Load(const boost::filesystem::path& name, ArgT... args) {
		auto it = assets.find(name.generic_string());
		if(it != assets.end()) {
			return *reinterpret_cast<T*>(it->second.get());
		}

		// Load asset from scratch.
		auto loader = T::GetLoader(std::forward(args)...);
		return *reinterpret_cast<T*>(InternalLoad(name, loader));
	}
};

}
}
