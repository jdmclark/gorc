#include "update_slide_ceiling_sky_aspect.hpp"
#include "game/constants.hpp"

gorc::game::world::animations::aspects::update_slide_ceiling_sky_aspect::update_slide_ceiling_sky_aspect(entity_component_system& cs, level_model& model)
    : inner_join_aspect(cs), model(model) {
    return;
}

void gorc::game::world::animations::aspects::update_slide_ceiling_sky_aspect::update(time_delta t, thing_id, components::slide_ceiling_sky& anim) {
    model.header.ceiling_sky_offset += anim.speed * static_cast<float>(t.count() * rate_factor);
}
