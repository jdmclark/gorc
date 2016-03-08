#pragma once

#include "libold/content/assets/level_surface.hpp"
#include "utility/maybe.hpp"
#include "physics/object_data.hpp"

namespace gorc {
namespace game {
namespace world {

namespace animations {
class animation;
}

class surface : public content::assets::level_surface {
public:
    physics::surface_object_data object_data;

    int cel_number = -1;

    inline const physics::surface_object_data& get_object_data() const {
        return object_data;
    }

    surface(const content::assets::level_surface& surf);
};

}
}
}
