#include "instance.hpp"
#include "log/log.hpp"

gorc::cog::instance::instance(asset_ref<script> cog)
    : cog(cog)
{
    for(auto const &sym : cog->symbols) {
        memory[sym.sequence_number] = sym.default_value;
    }

    return;
}

gorc::cog::instance::instance(asset_ref<script> cog,
                              std::vector<value> const &values)
    : cog(cog)
{
    auto vt = values.begin();
    for(auto const &sym : cog->symbols) {
        if(sym.local) {
            memory[sym.sequence_number] = sym.default_value;
        }
        else if(vt == values.end()) {
            LOG_FATAL("not enough values to initialize heap");
        }
        else {
            memory[sym.sequence_number] = *vt;
            ++vt;
        }
    }
}
