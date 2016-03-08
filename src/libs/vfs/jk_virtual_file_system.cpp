#include "jk_virtual_file_system.hpp"
#include "log/log.hpp"
#include "vfs/gob_virtual_container.hpp"
#include "io/native_file.hpp"
#include "utility/range.hpp"
#include "utility/string_search.hpp"
#include "utility/string_view.hpp"
#include <boost/filesystem.hpp>

using namespace gorc;

namespace {
    void load_gob(path const &filename,
                  vfs_map &map,
                  std::vector<std::unique_ptr<virtual_container>> &ctrs)
    {
        std::string ctr_filename = filename.generic_string();
        diagnostic_context dc(ctr_filename.c_str());

        std::unique_ptr<virtual_container> cnt;

        try {
            cnt = std::make_unique<gob_virtual_container>(filename);
        }
        catch(...) {
            // File is corrupt.
            // Error already reported, skip.
            return;
        }

        for(auto const &file : *cnt) {
            map.emplace(file.name.generic_string(), &file);
        }

        ctrs.push_back(std::move(cnt));
    }

    void load_gobs(path const &base_path,
                   vfs_map &map,
                   std::vector<std::unique_ptr<virtual_container>> &ctrs)
    {
        if(boost::filesystem::exists(base_path) &&
           boost::filesystem::is_directory(base_path)) {
            for(boost::filesystem::directory_iterator dir_iter(base_path);
                dir_iter != boost::filesystem::directory_iterator();
                ++dir_iter) {
                if(boost::filesystem::is_regular_file(dir_iter->status()) &&
                   iequal(dir_iter->path().extension().generic_string(), ".gob"_sv)) {
                    load_gob(dir_iter->path(), map, ctrs);
                }
            }
        }

        return;
    }

    std::unique_ptr<input_stream> find_in_gobs(path const &orig_path,
                                               path const &base_path,
                                               std::string const &generic_p,
                                               vfs_map const &map)
    {
        path in_bare_directory = base_path / orig_path;
        if(boost::filesystem::exists(in_bare_directory) &&
           boost::filesystem::is_regular_file(in_bare_directory)) {
            return make_native_read_only_file(in_bare_directory);
        }

        auto in_gob = map.find(generic_p);
        if(in_gob != map.end()) {
            return in_gob->second->open();
        }

        return nullptr;
    }
}

gorc::jk_virtual_file_system::jk_virtual_file_system(path const &resource_path)
    : resource_path(resource_path)
{
    load_gobs(resource_path, resource_file_map, containers);
    return;
}

gorc::jk_virtual_file_system::jk_virtual_file_system(path const &resource_path,
                                                     path const &game_path)
    : resource_path(resource_path)
    , game_path(game_path)
{
    load_gobs(resource_path, resource_file_map, containers);
    load_gobs(game_path, game_file_map, containers);
    return;
}

void gorc::jk_virtual_file_system::set_current_episode(virtual_container const &episode_ctr)
{
    episode_file_map.clear();
    for(auto const &file : episode_ctr) {
        episode_file_map.emplace(file.name.generic_string(), &file);
    }
}

std::tuple<gorc::path, std::unique_ptr<gorc::input_stream>>
    gorc::jk_virtual_file_system::find(path const &original_p,
                                       std::vector<path> const &prefixes) const
{
    for(auto const &prefix : prefixes) {
        path p = prefix / original_p;
        std::string generic_p = p.generic_string();
        std::transform(generic_p.begin(), generic_p.end(), generic_p.begin(), tolower);
        path normalized_p(generic_p);

        // Try from the current episode:
        auto ep_fn_it = episode_file_map.find(generic_p);
        if(ep_fn_it != episode_file_map.end()) {
            return std::make_tuple(p, ep_fn_it->second->open());
        }

        // Try from the current mod:
        if(game_path.has_value()) {
            auto game_file = find_in_gobs(normalized_p,
                                          game_path.get_value(),
                                          generic_p,
                                          game_file_map);
            if(game_file) {
                return std::make_tuple(p, std::move(game_file));
            }
        }

        // Try from resources
        auto res_file = find_in_gobs(normalized_p,
                                     resource_path,
                                     generic_p,
                                     resource_file_map);
        if(res_file) {
            return std::make_tuple(p, std::move(res_file));
        }
    }

    // Not found
    diagnostic_context dc(original_p.c_str());
    LOG_FATAL("file not found");
}

std::unique_ptr<gorc::input_stream> gorc::jk_virtual_file_system::open(path const &p) const
{
    return std::get<1>(find(p, { path() }));
}

namespace {
    void list_from_gob(std::map<std::string, std::string> &out_map,
                       vfs_map const &vm)
    {
        for(auto const &vf : vm) {
            out_map.emplace(vf.second->name.generic_string(),
                            vf.second->get_parent_container().container_filename.generic_string());
        }
    }

    path strip_first(path const &in_path, path const &root_path)
    {
        if(in_path.empty() || root_path.empty() || in_path == root_path) {
            return path();
        }
        else {
            return strip_first(in_path.parent_path(), root_path) / in_path.filename();
        }
    }

    void list_from_dir(std::map<std::string, std::string> &out_map,
                       path const &base_path)
    {
        if(boost::filesystem::exists(base_path) &&
           boost::filesystem::is_directory(base_path)) {
            for(boost::filesystem::directory_iterator dir_iter(base_path);
                dir_iter != boost::filesystem::directory_iterator();
                ++dir_iter) {
                if(boost::filesystem::is_regular_file(dir_iter->status())) {
                    auto p = dir_iter->path();
                    out_map.emplace(strip_first(dir_iter->path(), base_path).generic_string(),
                                    base_path.generic_string());
                }
            }
        }

        return;
    }
}

std::map<std::string, std::string> gorc::jk_virtual_file_system::list_files() const
{
    std::map<std::string, std::string> rv;

    list_from_gob(rv, episode_file_map);

    maybe_if(game_path, [&](path const &game_path) {
            list_from_dir(rv, game_path);
            list_from_gob(rv, game_file_map);
        });

    list_from_dir(rv, resource_path);
    list_from_gob(rv, resource_file_map);

    return rv;
}
