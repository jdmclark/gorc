#include "content_manager.h"
#include "loader.h"
#include "base/diagnostics/helper.h"
#include "base/io/exception.h"
#include <boost/format.hpp>
using namespace std;

gorc::content::content_manager::content_manager(diagnostics::report& report, const io::file_system& fs)
    : report(report), fs(fs) {
    return;
}

std::tuple<int, gorc::content::asset*> gorc::content::content_manager::internal_load(const boost::filesystem::path& name,
        const std::vector<boost::filesystem::path>& basepaths, loader& loader) {
    std::unique_ptr<io::read_only_file> file;
    boost::filesystem::path target_path;

    for(const auto& root : basepaths) {
        try {
            target_path = root / name;
            file = fs.open(target_path);

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
        assets.push_back(loader.deserialize(target_path, *file, *this, report));
        asset_map.insert(std::make_pair(name.generic_string(), assets.size() - 1));
        return std::make_tuple(assets.size() - 1, assets.back().get());
    }
    catch(const std::exception&) {
        diagnostics::helper::could_not_load_file(report, "contentmanager", name.generic_string());
        throw;
    }
}
