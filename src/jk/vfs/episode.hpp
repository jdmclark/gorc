#pragma once

#include "io/input_stream.hpp"
#include "episode_entry.hpp"
#include "episode_type.hpp"
#include "vfs/virtual_container.hpp"
#include <string>
#include <unordered_map>

namespace gorc {

    class episode {
    private:
        std::unordered_map<int, size_t> entry_map;
        std::vector<episode_entry> entries;

        std::string episode_name;
        episode_type type;

    public:
        episode(input_stream &);
        std::string const& get_episode_name() const;
        episode_type get_type() const;
        size_t size() const;

        inline auto begin() const -> decltype(entries.begin())
        {
            return entries.begin();
        }

        inline auto end() const -> decltype(entries.end())
        {
            return entries.end();
        }
    };

}
