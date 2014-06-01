#pragma once

#include "base/utility/inner_join_aspect.h"
#include "game/world/level_presenter.h"
#include "game/world/components/thing.h"
#include "game/world/components/class_sounds.h"
#include "content/flags/key_marker_type.h"
#include "base/utility/enum_hasher.h"

namespace gorc {
namespace game {
namespace world {
namespace aspects {

class dispatch_class_sound_aspect : public inner_join_aspect<components::class_sounds, components::thing> {
public:
    using standing_material_map = std::unordered_map<flags::standing_material_type,
                                                     flags::sound_subclass_type,
                                                     enum_hasher<flags::standing_material_type>>;
private:
    level_presenter &presenter;

    void dispatch_from_map(standing_material_map const&, entity_id, flags::standing_material_type);
    void handle_animation_marker(entity_id, components::class_sounds&, flags::key_marker_type);

public:
    dispatch_class_sound_aspect(component_system &cs, level_presenter &presenter);
};

}
}
}
}
