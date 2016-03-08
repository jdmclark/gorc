#include "content_manager.hpp"
#include "loader.hpp"
#include "log/log.hpp"

gorc::content::content_manager::content_manager(service_registry const &services)
    : services(services)
    , fs(services.get<virtual_file_system>()) {
    return;
}

std::tuple<int, gorc::asset*> gorc::content::content_manager::internal_load(const boost::filesystem::path& name,
        const std::vector<boost::filesystem::path>& basepaths, loader& loader) {
    std::string generic_name = name.generic_string();
    diagnostic_context dc(generic_name.c_str());

    path target_path;
    std::unique_ptr<input_stream> file;
    std::tie(target_path, file) = fs.find(name, basepaths);

    assets.push_back(loader.deserialize(*file, *this, services));
    asset_map.insert(std::make_pair(name.generic_string(), assets.size() - 1));
    return std::make_tuple(assets.size() - 1, assets.back().get());
}
