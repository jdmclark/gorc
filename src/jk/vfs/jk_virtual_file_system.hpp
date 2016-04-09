#pragma once

#include "vfs/virtual_file_system.hpp"
#include "vfs/virtual_container.hpp"
#include "utility/maybe.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <iosfwd>
#include <map>

namespace gorc {

    typedef std::unordered_map<std::string, virtual_file const *> vfs_map;

    class jk_virtual_file_system : public virtual_file_system {
    private:
        path const resource_path;
        maybe<path> const game_path;

        vfs_map resource_file_map;
        vfs_map game_file_map;
        vfs_map episode_file_map;

        std::vector<std::unique_ptr<virtual_container>> containers;

    public:
        jk_virtual_file_system(path const &resource_path);
        jk_virtual_file_system(path const &resource_path,
                               path const &game_path);

        void set_current_episode(virtual_container const &episode_ctr);

        virtual std::unique_ptr<input_stream> open(path const &filename) const override;
        virtual std::tuple<path, std::unique_ptr<input_stream>>
            find(path const &filename, std::vector<path> const &prefixes) const override;

        std::map<std::string, std::string> list_files() const;
    };

}
