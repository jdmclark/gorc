#pragma once

namespace gorc {

    enum class episode_entry_type {
        cutscene,
        level,
        decision
    };

    char const* as_string(episode_entry_type);

}
