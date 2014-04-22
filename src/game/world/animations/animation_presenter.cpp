#include "cog/verbs/table.h"
#include "game/level_state.h"
#include "animation_presenter.h"
#include "animation_model.h"
#include "game/world/level_model.h"
#include "slide_ceiling_sky_animation.h"
#include "slide_surface_animation.h"
#include "surface_material_animation.h"
#include "surface_light_animation.h"
#include "game/world/level_presenter.h"

using namespace gorc::game::world::animations;

void animation_presenter::start(level_model& levelModel, animation_model& model) {
    this->levelModel = &levelModel;
    this->model = &model;
}

void animation_presenter::update(const time& time) {
    for(auto& entity : model->animations) {
            entity->update(time.elapsed_as_seconds());
    }

    for(auto& anim : model->animations) {
        if(anim->expired) {
            model->animations.erase(anim);
        }
    }
}

// Anim / Cel verbs
int animation_presenter::surface_anim(int surface, float rate, flag_set<flags::AnimFlag> flags) {
    auto& ent = model->animations.emplace();
    ent.value = make_unique<surface_material_animation>(*levelModel, surface, rate, flags, ent.get_id());
    ent.value->set_id(ent.get_id());
    return ent.get_id();
}

int animation_presenter::get_surface_anim(int surface) {
    int return_value = -1;
    if(animation const* anim = levelModel->surfaces[surface].surface_anim) {
        return_value = anim->get_id();
    }

    return return_value;
}

void animation_presenter::stop_anim(int anim) {
    if(anim >= 0) {
        auto& anim_ref = model->animations[anim];
        anim_ref->stop();
        model->animations.erase(anim_ref);
    }
}

int animation_presenter::get_surface_cel(int surface) {
    return levelModel->surfaces[surface].cel_number;
}

void animation_presenter::set_surface_cel(int surface, int cel) {
    levelModel->surfaces[surface].cel_number = cel;
}

int animation_presenter::slide_surface(int surface_id, const vector<3>& direction) {
    auto& ent = model->animations.emplace();
    ent.value = make_unique<slide_surface_animation>(*levelModel, surface_id, direction, ent.get_id());
    ent.value->set_id(ent.get_id());
    return ent.get_id();
}

int animation_presenter::slide_ceiling_sky(float u_speed, float v_speed) {
    auto& ent = model->animations.emplace();
    ent.value = make_unique<slide_ceiling_sky_animation>(*levelModel, make_vector(u_speed, v_speed));
    ent.value->set_id(ent.get_id());
    return ent.get_id();
}

int animation_presenter::surface_light_anim(int surface, float start_light, float end_light, float change_time) {
    auto& ent = model->animations.emplace();
    ent.value = make_unique<surface_light_animation>(*levelModel, surface, start_light, end_light, change_time, ent.get_id());
    ent.value->set_id(ent.get_id());
    return ent.get_id();
}

void gorc::game::world::animations::animation_presenter::register_verbs(cog::verbs::verb_table& verbTable, level_state& components) {
    verbTable.add_verb<int, 3>("materialanim", [&components](int, float, int) {
        // TODO: Implement this verb when setsurfacematerial changes have been made.
        return -1;
    });

    verbTable.add_verb<int, 1>("getsurfaceanim", [&components](int surface) {
        return static_cast<int>(components.current_level_presenter->animation_presenter->get_surface_anim(surface));
    });

    verbTable.add_verb<void, 1>("stopsurfaceanim", [&components](int surface) {
        components.current_level_presenter->animation_presenter->stop_anim(
                components.current_level_presenter->animation_presenter->get_surface_anim(surface));
    });

    verbTable.add_verb<int, 1>("getwallcel", [&components](int surface) {
        return components.current_level_presenter->animation_presenter->get_surface_cel(surface);
    });

    verbTable.add_verb<int, 2>("setwallcel", [&components](int surface, int cel) {
        components.current_level_presenter->animation_presenter->set_surface_cel(surface, cel);
        return 1;
    });

    verbTable.add_verb<int, 3>("surfaceanim", [&components](int surface, float rate, int flags) {
        return static_cast<int>(components.current_level_presenter->animation_presenter->surface_anim(surface, rate, flag_set<flags::AnimFlag>(flags)));
    });

    verbTable.add_verb<int, 2>("slideceilingsky", [&components](float u_speed, float v_speed) {
        return static_cast<int>(components.current_level_presenter->animation_presenter->slide_ceiling_sky(u_speed, v_speed));
    });

    verbTable.add_verb<int, 3>("slidesurface", [&components](int surface, vector<3> direction, float speed) {
        return static_cast<int>(components.current_level_presenter->animation_presenter->slide_surface(surface, normalize(direction) * speed));
    });

    verbTable.add_verb<int, 3>("slidewall", [&components](int surface, vector<3> direction, float speed) {
        return static_cast<int>(components.current_level_presenter->animation_presenter->slide_surface(surface, normalize(direction) * speed));
    });

    verbTable.add_verb<void, 1>("stopanim", [&components](int anim) {
        components.current_level_presenter->animation_presenter->stop_anim(anim);
    });

    verbTable.add_verb<int, 4>("surfacelightanim", [&components](int surface, float start_light, float end_light, float change_time) {
        return static_cast<int>(components.current_level_presenter->animation_presenter->surface_light_anim(surface, start_light, end_light, change_time));
    });
}
