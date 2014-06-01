#include "dispatch_class_sound_aspect.h"
#include "game/world/events/class_sound.h"
#include "game/world/events/animation_marker.h"
#include "game/world/events/standing_material_changed.h"
#include "game/world/events/jumped.h"
#include "game/world/events/landed.h"
#include "game/world/events/killed.h"
#include "game/world/events/thing_created.h"
#include "base/utility/enum_hasher.h"
#include "game/world/sounds/sound_presenter.h"

#include <unordered_map>
#include <tuple>

using gorc::game::world::aspects::dispatch_class_sound_aspect;
using namespace gorc::flags;

// Static walk material -> soundclass maps
dispatch_class_sound_aspect::standing_material_map const character_left_run_map {
    { standing_material_type::hard, sound_subclass_type::LRunHard },
    { standing_material_type::dirt, sound_subclass_type::LRunEarth },
    { standing_material_type::metal, sound_subclass_type::LRunMetal },
    { standing_material_type::shallow_water, sound_subclass_type::LRunPuddle },
    { standing_material_type::deep_water, sound_subclass_type::LRunWater },
    { standing_material_type::very_deep_water, sound_subclass_type::LRunWater }
};

dispatch_class_sound_aspect::standing_material_map const character_right_run_map {
    { standing_material_type::hard, sound_subclass_type::RRunHard },
    { standing_material_type::dirt, sound_subclass_type::RRunEarth },
    { standing_material_type::metal, sound_subclass_type::RRunMetal },
    { standing_material_type::shallow_water, sound_subclass_type::RRunPuddle },
    { standing_material_type::deep_water, sound_subclass_type::RRunWater },
    { standing_material_type::very_deep_water, sound_subclass_type::RRunWater }
};

dispatch_class_sound_aspect::standing_material_map const character_left_walk_map {
    { standing_material_type::hard, sound_subclass_type::LWalkHard },
    { standing_material_type::dirt, sound_subclass_type::LWalkEarth },
    { standing_material_type::metal, sound_subclass_type::LWalkMetal },
    { standing_material_type::shallow_water, sound_subclass_type::LWalkPuddle },
    { standing_material_type::deep_water, sound_subclass_type::LWalkWater },
    { standing_material_type::very_deep_water, sound_subclass_type::LWalkWater }
};

dispatch_class_sound_aspect::standing_material_map const character_right_walk_map {
    { standing_material_type::hard, sound_subclass_type::RWalkHard },
    { standing_material_type::dirt, sound_subclass_type::RWalkEarth },
    { standing_material_type::metal, sound_subclass_type::RWalkMetal },
    { standing_material_type::shallow_water, sound_subclass_type::RWalkPuddle },
    { standing_material_type::deep_water, sound_subclass_type::RWalkWater },
    { standing_material_type::very_deep_water, sound_subclass_type::RWalkWater }
};

dispatch_class_sound_aspect::standing_material_map const character_land_map {
    { standing_material_type::hard, sound_subclass_type::LandHard },
    { standing_material_type::dirt, sound_subclass_type::LandEarth },
    { standing_material_type::metal, sound_subclass_type::LandMetal },
    { standing_material_type::shallow_water, sound_subclass_type::LandPuddle },
    { standing_material_type::deep_water, sound_subclass_type::LandWater },
    { standing_material_type::very_deep_water, sound_subclass_type::LandWater }
};

dispatch_class_sound_aspect::standing_material_map const character_jump_map {
    { standing_material_type::hard, sound_subclass_type::Jump },
    { standing_material_type::dirt, sound_subclass_type::JumpEarth },
    { standing_material_type::metal, sound_subclass_type::JumpMetal },
    { standing_material_type::shallow_water, sound_subclass_type::JumpWater },
    { standing_material_type::deep_water, sound_subclass_type::JumpWater },
    { standing_material_type::very_deep_water, sound_subclass_type::JumpWater }
};

void dispatch_class_sound_aspect::dispatch_from_map(standing_material_map const &m,
                                                    entity_id thing,
                                                    standing_material_type t) {
    auto em = m.find(t);
    if(em != m.end()) {
        presenter.sound_presenter->play_sound_class(thing, em->second);
    }
}

void dispatch_class_sound_aspect::handle_animation_marker(entity_id id,
                                                          components::class_sounds &csnd,
                                                          flags::key_marker_type type) {
    switch(type) {
    case key_marker_type::LeftRunFootstep:
        dispatch_from_map(character_left_run_map, id, csnd.standing_material_type);
        break;

    case key_marker_type::RightRunFootstep:
        dispatch_from_map(character_right_run_map, id, csnd.standing_material_type);
        break;

    case key_marker_type::LeftFootstep:
        dispatch_from_map(character_left_walk_map, id, csnd.standing_material_type);
        break;

    case key_marker_type::RightFootstep:
        dispatch_from_map(character_right_walk_map, id, csnd.standing_material_type);
        break;

    case key_marker_type::Death:
        presenter.sound_presenter->play_sound_class(id, sound_subclass_type::CorpseHit);
        break;

    default:
        // TODO: Handle other animation markers.
        break;
    }
}

dispatch_class_sound_aspect::dispatch_class_sound_aspect(component_system &cs,
                                                         level_presenter &presenter)
    : inner_join_aspect(cs), presenter(presenter) {

    cs.bus.add_handler<events::thing_created>([&](events::thing_created &e) {
        if(e.tpl.sound_class) {
            cs.emplace_component<components::class_sounds>(e.thing, *e.tpl.sound_class);

            presenter.sound_presenter->play_sound_class(e.thing,
                                                        flags::sound_subclass_type::create);
        }
    });

    cs.bus.add_handler<events::standing_material_changed>([&](events::standing_material_changed &e) {
        for(auto &csnd : cs.find_component<components::class_sounds>(e.thing)) {
            csnd.second.standing_material_type = e.type;
        }
    });

    cs.bus.add_handler<events::class_sound>([&](events::class_sound &e) {
        for(auto &csnd : cs.find_component<components::class_sounds>(e.thing)) {
            presenter.sound_presenter->play_sound_class(e.thing, e.type);
        }
    });

    cs.bus.add_handler<events::animation_marker>([&](events::animation_marker &e) {
        for(auto &csnd : cs.find_component<components::class_sounds>(e.thing)) {
            handle_animation_marker(e.thing, csnd.second, e.type);
        }
    });

    cs.bus.add_handler<events::jumped>([&](events::jumped &e) {
        for(auto &csnd : cs.find_component<components::class_sounds>(e.thing)) {
            dispatch_from_map(character_jump_map, e.thing, csnd.second.standing_material_type);
        }
    });

    cs.bus.add_handler<events::landed>([&](events::landed &e) {
        for(auto &csnd : cs.find_component<components::class_sounds>(e.thing)) {
            dispatch_from_map(character_land_map, e.thing, csnd.second.standing_material_type);
        }
    });

    cs.bus.add_handler<events::killed>([&](events::killed &e) {
        for(auto &csnd : cs.find_component<components::class_sounds>(e.thing)) {
            double random_value = static_cast<double>(rand);

            if(random_value < 0.5) {
                presenter.sound_presenter->play_sound_class(e.thing, sound_subclass_type::Death1);
            }
            else {
                presenter.sound_presenter->play_sound_class(e.thing, sound_subclass_type::Death2);
            }
        }
    });

    return;
}
