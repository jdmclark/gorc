#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <map>
#include "base/math/vector.h"
#include "base/content/asset.h"
#include "content/loaders/inventory_loader.h"
#include "inventory_bin.h"

namespace gorc {
namespace content {
namespace assets {

class inventory : public asset {
public:
    using loader = loaders::inventory_loader;

    std::unordered_map<std::string, int> bin_map;
    std::map<int, inventory_bin> bins;

    inline const inventory_bin& get_bin(const std::string& name) const {
        auto it = bin_map.find(name);
        if(it != bin_map.end()) {
            auto jt = bins.find(it->second);
            if(jt != bins.end()) {
                return jt->second;
            }
        }

        throw std::exception();
    }

    inline const inventory_bin& get_bin(int id) const {
        auto it = bins.find(id);
        if(it != bins.end()) {
            return it->second;
        }

        throw std::exception();
    }

    inline auto begin() const -> decltype(bins.begin()) {
        return bins.begin();
    }

    inline auto end() const -> decltype(bins.end()) {
        return bins.end();
    }
};

}
}
}
