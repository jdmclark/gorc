#pragma once

#include "framework/io/read_only_file.h"
#include "framework/diagnostics/report.h"
#include "entry.h"
#include "episode_type.h"
#include "content/vfs/container.h"
#include <string>
#include <unordered_map>

namespace gorc {
namespace content {
namespace vfs {
namespace episode {

class episode {
private:
    std::unordered_map<int, size_t> entryMap;
    std::vector<entry> entries;

    std::string EpisodeName;
    episode_type type;

public:
    episode(io::read_only_file& file, diagnostics::report& report);

    inline const std::string& get_episode_name() const {
        return EpisodeName;
    }

    inline episode_type get_type() const {
        return type;
    }

    inline auto begin() const -> decltype(entries.begin()) {
        return entries.begin();
    }

    inline auto end() const -> decltype(entries.end()) {
        return entries.end();
    }

    const entry& get_entry(int LineNumber) const;
};

}
}
}
}
