#pragma once

#include "libold/base/utility/inner_join_aspect.hpp"
#include "game/world/level_presenter.hpp"
#include "game/world/components/thing.hpp"
#include "game/world/components/class_sounds.hpp"
#include "libold/content/flags/key_marker_type.hpp"
#include "libold/base/utility/randomizer.hpp"
#include "utility/enum_hash.hpp"

namespace gorc {
namespace game {
namespace world {
namespace aspects {

class dispatch_class_sound_aspect : public inner_join_aspect<components::class_sounds, components::thing> {
public:
    using standing_material_map = std::unordered_map<flags::standing_material_type,
                                                     flags::sound_subclass_type,
                                                     enum_hash<flags::standing_material_type>>;
private:
    level_presenter &presenter;
    randomizer rand;

    maybe<scoped_delegate> created_delegate;
    maybe<scoped_delegate> material_change_delegate;
    maybe<scoped_delegate> class_sound_delegate;
    maybe<scoped_delegate> animation_marker_delegate;
    maybe<scoped_delegate> jumped_delegate;
    maybe<scoped_delegate> landed_delegate;
    maybe<scoped_delegate> killed_delegate;

    void dispatch_from_map(standing_material_map const&, int, flags::standing_material_type);
    void handle_animation_marker(int, components::class_sounds&, flags::key_marker_type);

public:
    dispatch_class_sound_aspect(component_system &cs, level_presenter &presenter);
};

}
}
}
}
