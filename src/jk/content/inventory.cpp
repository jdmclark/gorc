#include "inventory.hpp"
#include "log/log.hpp"

gorc::fourcc const gorc::inventory::type = "IBIN"_4CC;

gorc::inventory_bin const& gorc::inventory::get_bin(std::string const &name) const
{
    auto it = bin_map.find(name);
    if(it != bin_map.end()) {
        return get_bin(it->second);
    }

    LOG_FATAL(format("inventory bin %s not found") % name);
}

gorc::inventory_bin const& gorc::inventory::get_bin(int id) const
{
    auto it = bins.find(id);
    if(it != bins.end()) {
        return it->second;
    }

    LOG_FATAL(format("inventory bin %d not found") % id);
}
