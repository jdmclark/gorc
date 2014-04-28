#include "surface_animation_cleanup_aspect.h"

#include "game/world/animations/events/stop_animation.h"

gorc::game::world::animations::aspects::surface_animation_cleanup_aspect::surface_animation_cleanup_aspect(component_system& cs) {

    cs.bus.add_handler<events::stop_animation>([&cs](const events::stop_animation& e) {
        auto range = cs.all_components<components::surface_animation>();
        for(auto it = range.begin(); it != range.end();) {
            auto jt = it++;

            if(jt->second.animation == e.animation) {
                cs.erase_component(jt);
            }
        }
    });

    return;
}
