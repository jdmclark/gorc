#include "manager.h"
#include "loader.h"
#include "framework/diagnostics/helper.h"
#include "framework/io/exception.h"
#include <boost/format.hpp>

gorc::content::manager::manager(diagnostics::report& report, const filesystem& fs)
    : report(report), fs(fs) {
    return;
}

std::tuple<int, gorc::content::asset*> gorc::content::manager::InternalLoad(const boost::filesystem::path& name,
        const std::vector<boost::filesystem::path>& basepaths, loader& loader) {
    std::unique_ptr<io::read_only_file> file;

    for(const auto& root : basepaths) {
        try {
            file = fs.open(root / name);
            break;
        }
        catch(...) {
            continue;
        }
    }

    if(!file) {
        diagnostics::helper::file_not_found(report, "contentmanager", name.generic_string());
        throw io::file_not_found_exception();
    }

    try {
        assets.push_back(loader.deserialize(*file, *this, report));
        asset_map.insert(std::make_pair(name.generic_string(), assets.size() - 1));
        return std::make_tuple(assets.size() - 1, assets.back().get());
    }
    catch(const std::exception&) {
        diagnostics::helper::could_not_load_file(report, "contentmanager", file->Filename.generic_string());
        throw;
    }
}
