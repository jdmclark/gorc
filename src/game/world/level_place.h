#pragma once

#include "content/assets/level.h"
#include "base/content/content_manager.h"
#include <memory>

namespace gorc {
namespace game {
namespace world {

class level_place {
public:
    std::shared_ptr<content::content_manager> contentmanager;
    const content::assets::level& level;

    level_place(std::shared_ptr<content::content_manager> contentmanager, const content::assets::level& level);
};

}
}
}
