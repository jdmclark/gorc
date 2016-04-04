#include "update_slide_surface_aspect.hpp"
#include "game/world/animations/events/stop_animation.hpp"
#include "game/constants.hpp"

gorc::game::world::animations::aspects::update_slide_surface_aspect::update_slide_surface_aspect(component_system& cs, level_model& model)
    : inner_join_aspect(cs), model(model) {

    // Add event handler to reduce thrust to 0 when the animation has stopped
    stop_animation_delegate =
        cs.bus.add_handler<events::stop_animation>([&cs, &model](const events::stop_animation& e) {
        for(auto& anim : cs.find_component<components::slide_surface>(entity_id(e.animation))) {
            model.surfaces[anim.second.surface].thrust = make_zero_vector<3, float>();
        }
    });

    return;
}

void gorc::game::world::animations::aspects::update_slide_surface_aspect::update(gorc::time t, entity_id, components::slide_surface& anim) {
    auto& surf = model.surfaces[static_cast<int>(anim.surface)];
    surf.thrust = anim.direction * static_cast<float>(rate_factor);

    auto plane_dir = anim.tb0 * dot(anim.direction, anim.sb0) + anim.tb1 * dot(anim.direction, anim.sb1);

    surf.texture_offset += plane_dir * static_cast<float>(t.elapsed_as_seconds() * rate_factor);
}
