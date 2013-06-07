#include "manager.h"
#include "loader.h"
#include "framework/diagnostics/helper.h"
#include "framework/io/exception.h"
#include <boost/format.hpp>

Gorc::Content::Manager::Manager(Diagnostics::Report& report, const FileSystem& fs)
	: report(report), fs(fs) {
	return;
}

Gorc::Content::Asset* Gorc::Content::Manager::InternalLoad(const boost::filesystem::path& name,
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
		return assets.insert(std::make_pair(name.generic_string(), loader.Deserialize(*file, *this, report))).first->second.get();
	}
	catch(const std::exception&) {
		Diagnostics::Helper::CouldNotLoadFile(report, "ContentManager", file->Filename.generic_string());
		throw;
	}
}
