#pragma once

#include "inventory_bin.hpp"
#include <unordered_map>

namespace gorc {

    class inventory : public asset {
    public:
        static fourcc const type;

        std::unordered_map<std::string, int> bin_map;
        std::unordered_map<int, inventory_bin> bins;

        inventory_bin const& get_bin(std::string const &name) const;
        inventory_bin const& get_bin(int id) const;

        inline auto begin() const
        {
            return bins.begin();
        }

        inline auto end() const
        {
            return bins.end();
        }
    };

}
