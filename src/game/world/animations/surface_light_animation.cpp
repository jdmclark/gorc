#include "surface_light_animation.h"
#include "game/world/level_model.h"

gorc::game::world::animations::surface_light_animation::surface_light_animation(level_model& model, unsigned int surface, float start_light,
        float end_light, float change_time, int anim_num)
    : model(model), surface(surface), start_light(start_light), end_light(end_light), change_time(change_time), anim_time(0.0) {
    model.surfaces[surface].extra_light = start_light;
    model.surfaces[surface].surface_anim = this;
    return;
}

void gorc::game::world::animations::surface_light_animation::update(double dt) {
    // TODO: It may have been LEC's intention for this animation to loop.
    // Test it out, see if it was a bug all along.
    anim_time += static_cast<float>(dt);
    float alpha = std::min(anim_time / change_time, 1.0f);
    model.surfaces[surface].extra_light = (1.0f - alpha) * start_light + alpha * end_light;
}

void gorc::game::world::animations::surface_light_animation::stop() {
    auto& surf = model.surfaces[surface];
    model.surfaces[surface].surface_anim = maybe<animation*>();
}
