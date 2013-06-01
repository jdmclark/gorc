#include "manager.h"
#include "loader.h"
#include "framework/diagnostics/helper.h"
#include <boost/format.hpp>

Gorc::Content::Manager::Manager(Diagnostics::Report& report, FileSystem& fs)
	: report(report), fs(fs) {
	return;
}

Gorc::Content::Asset* Gorc::Content::Manager::InternalLoad(const boost::filesystem::path& name, Loader& loader) {
	std::unique_ptr<IO::ReadOnlyFile> file;

	try {
		file = fs.Open(name);
	}
	catch(const std::exception&) {
		Diagnostics::Helper::FileNotFound(report, "ContentManager", name.generic_string());
		throw;
	}

	try {
		return assets.insert(std::make_pair(name.generic_string(), loader.Deserialize(*file, *this, report))).first->second.get();
	}
	catch(const std::exception&) {
		Diagnostics::Helper::CouldNotLoadFile(report, "ContentManager", name.generic_string());
		throw;
	}
}
