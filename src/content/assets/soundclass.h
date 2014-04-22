#pragma once

#include "base/content/asset.h"
#include "content/loaders/soundclass_loader.h"
#include "soundsubclass.h"
#include <array>

namespace gorc {
namespace content {
namespace assets {

class soundclass : public asset {
public:
    using loader = loaders::soundclass_loader;

    std::array<sound_subclass, 128> subclasses;

    inline const sound_subclass& get(flags::sound_subclass_type type) const {
        return subclasses[static_cast<int>(type)];
    }
};

}
}
}
