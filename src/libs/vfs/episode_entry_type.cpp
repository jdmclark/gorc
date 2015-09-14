#include "episode_entry_type.hpp"
#include "utility/enum_hash.hpp"
#include <unordered_map>

using namespace gorc;

namespace {
    std::unordered_map<episode_entry_type, char const *, enum_hash<episode_entry_type>> type_map {
        { episode_entry_type::level, "level" },
        { episode_entry_type::cutscene, "cutscene" },
        { episode_entry_type::decision, "decision" }
    };
}

char const* gorc::as_string(episode_entry_type t)
{
    auto it = type_map.find(t);
    if(it != type_map.end()) {
        return it->second;
    }

    throw std::range_error("not a valid episode_entry_type");
}
