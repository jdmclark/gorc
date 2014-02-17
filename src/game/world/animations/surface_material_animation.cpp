#include "surface_material_animation.h"
#include "game/world/level_model.h"

gorc::game::world::animations::surface_material_animation::surface_material_animation(level_model& model, unsigned int surface,
        double framerate, flag_set<flags::AnimFlag> flag, int anim_num)
    : model(model), surface(surface), framerate(1.0 / framerate), flag(flag), framerate_accumulator(0.0) {

    int surface_material = model.level.surfaces[surface].material;
    if(surface_material < 0) {
        // TODO: surface has no material but has an animation? report error.
        num_cels = 0;
        return;
    }

    num_cels = std::get<0>(model.level.materials[surface_material])->cels.size();

    if(flag & flags::AnimFlag::SkipFirstTwoFrames) {
        model.surfaces[surface].cel_number = 2 % num_cels;
    }
    else if(flag & flags::AnimFlag::SkipFirstFrame) {
        model.surfaces[surface].cel_number = 1 % num_cels;
    }
    else {
        model.surfaces[surface].cel_number = 0;
    }

    model.surfaces[surface].surface_anim = this;
}

void gorc::game::world::animations::surface_material_animation::update(double dt) {
    framerate_accumulator += dt;

    while(framerate_accumulator >= framerate) {
        framerate_accumulator -= framerate;

        int next_cel = model.surfaces[surface].cel_number + 1;
        if(next_cel >= num_cels) {
            if(flag & flags::AnimFlag::SkipFirstTwoFrames) {
                next_cel = 2 % num_cels;
            }
            else if(flag & flags::AnimFlag::SkipFirstFrame) {
                next_cel = 1 % num_cels;
            }
            else {
                next_cel = 0;
            }
        }

        model.surfaces[surface].cel_number = next_cel;
    }
}

void gorc::game::world::animations::surface_material_animation::stop() {
    model.surfaces[surface].surface_anim = maybe<animation*>();
}
