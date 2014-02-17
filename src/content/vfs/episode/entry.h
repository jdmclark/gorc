#pragma once

#include "entry_type.h"
#include <boost/filesystem/path.hpp>

namespace gorc {
namespace content {
namespace vfs {
namespace episode {

class entry {
public:
    const int number;
    const int disc;
    const int level_number;
    const entry_type type;
    const boost::filesystem::path filename;
    const int light_power;
    const int dark_power;
    const int goto_a;
    const int goto_b;

    entry(int number, int Disc, int LevelNumber, entry_type type,
            const boost::filesystem::path& Filename, int LightPower, int DarkPower,
            int GotoA, int GotoB);
};

}
}
}
}
