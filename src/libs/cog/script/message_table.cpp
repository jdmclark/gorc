#include "message_table.hpp"

void gorc::cog::message_table::set_offset(message_type t, size_t o)
{
    offsets.emplace(t, o);
}

gorc::maybe<size_t> gorc::cog::message_table::get_offset(message_type t) const
{
    auto it = offsets.find(t);
    if(it != offsets.end()) {
        return it->second;
    }

    return nothing;
}
