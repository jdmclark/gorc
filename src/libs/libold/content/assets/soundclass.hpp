#pragma once

#include "content/asset.hpp"
#include "libold/content/loaders/soundclass_loader.hpp"
#include "soundsubclass.hpp"
#include <array>

namespace gorc {
namespace content {
namespace assets {

class soundclass : public asset {
public:
    static fourcc const type;

    std::array<sound_subclass, 128> subclasses;

    inline const sound_subclass& get(flags::sound_subclass_type type) const {
        return subclasses[static_cast<int>(type)];
    }
};

}
}
}
