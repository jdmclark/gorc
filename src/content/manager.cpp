#include "manager.h"
#include "loader.h"
#include "framework/diagnostics/helper.h"
#include "framework/io/exception.h"
#include <boost/format.hpp>

Gorc::Content::Manager::Manager(Diagnostics::Report& report, const FileSystem& fs)
	: report(report), fs(fs) {
	return;
}

std::tuple<int, Gorc::Content::Asset*> Gorc::Content::Manager::InternalLoad(const boost::filesystem::path& name,
		const std::vector<boost::filesystem::path>& basepaths, Loader& loader) {
	std::unique_ptr<IO::ReadOnlyFile> file;

	for(const auto& root : basepaths) {
		try {
			file = fs.Open(root / name);
			break;
		}
		catch(...) {
			continue;
		}
	}

	if(!file) {
		Diagnostics::Helper::FileNotFound(report, "ContentManager", name.generic_string());
		throw IO::FileNotFoundException();
	}

	try {
		assets.push_back(loader.Deserialize(*file, *this, report));
		asset_map.insert(std::make_pair(name.generic_string(), assets.size() - 1));
		return std::make_tuple(assets.size() - 1, assets.back().get());
	}
	catch(const std::exception&) {
		Diagnostics::Helper::CouldNotLoadFile(report, "ContentManager", file->Filename.generic_string());
		throw;
	}
}
