#include "game/level_state.hpp"
#include "animation_presenter.hpp"
#include "game/world/level_model.hpp"
#include "game/world/level_presenter.hpp"

#include "aspects/update_surface_material_aspect.hpp"
#include "aspects/update_slide_ceiling_sky_aspect.hpp"
#include "aspects/update_surface_light_aspect.hpp"
#include "aspects/update_slide_surface_aspect.hpp"

#include "components/surface_animation.hpp"
#include "components/surface_material.hpp"
#include "components/slide_ceiling_sky.hpp"
#include "components/surface_light.hpp"
#include "components/slide_surface.hpp"

#include "events/stop_animation.hpp"

using namespace gorc::game::world::animations;

void animation_presenter::start(level_model& levelModel) {
    this->levelModel = &levelModel;

    levelModel.ecs.emplace_aspect<aspects::update_surface_material_aspect>(levelModel);
    levelModel.ecs.emplace_aspect<aspects::update_slide_ceiling_sky_aspect>(levelModel);
    levelModel.ecs.emplace_aspect<aspects::update_slide_surface_aspect>(levelModel);
    levelModel.ecs.emplace_aspect<aspects::update_surface_light_aspect>(levelModel);
}

// Anim / Cel verbs
gorc::thing_id animation_presenter::surface_anim(surface_id surface, float rate, flag_set<flags::anim_flag> flag) {
    thing_id emplaced_anim = levelModel->ecs.emplace_entity();

    int surface_material = at_id(levelModel->level->surfaces, surface).material;
    if(surface_material < 0) {
        // TODO: surface has no material but has an animation? report error.
        return emplaced_anim;
    }

    size_t num_cels = std::get<0>(levelModel->level->materials[surface_material]).get_value()->cels.size();

    if(flag & flags::anim_flag::skip_first_two_frames) {
        at_id(levelModel->surfaces, surface).cel_number = static_cast<int>(2UL % num_cels);
    }
    else if(flag & flags::anim_flag::skip_first_frame) {
        at_id(levelModel->surfaces, surface).cel_number = static_cast<int>(1UL % num_cels);
    }
    else {
        at_id(levelModel->surfaces, surface).cel_number = 0;
    }

    levelModel->ecs.emplace_component<components::surface_material>(emplaced_anim, surface, 1.0 / static_cast<double>(rate), flag);
    levelModel->ecs.emplace_component<components::surface_animation>(emplaced_anim, surface);

    return emplaced_anim;
}

gorc::thing_id animation_presenter::get_surface_anim(surface_id surface) {
    for(auto& surf_anim : levelModel->ecs.all_components<components::surface_animation>()) {
        if(surf_anim.second->surface == surface) {
            return surf_anim.first;
        }
    }

    return invalid_id;
}

void animation_presenter::stop_surface_anim(surface_id surface) {
    auto cmp_range = levelModel->ecs.all_components<components::surface_animation>();
    for(auto it = cmp_range.begin(); it != cmp_range.end();) {
        auto jt = it++;
        if(jt->second->surface == surface) {
            stop_anim(jt->first);
        }
    }
}

void animation_presenter::stop_anim(thing_id anim_id) {
    levelModel->ecs.bus.fire_event(events::stop_animation(anim_id));
    levelModel->ecs.erase_entity(anim_id);
}

int animation_presenter::get_surface_cel(surface_id surface) {
    return at_id(levelModel->surfaces, surface).cel_number;
}

void animation_presenter::set_surface_cel(surface_id surface, int cel) {
    at_id(levelModel->surfaces, surface).cel_number = cel;
}

gorc::thing_id animation_presenter::slide_surface(surface_id surface, const vector<3>& direction) {
    // Compute surface slide direction
    auto &surf = at_id(levelModel->surfaces, surface);

    auto dnsb0 = levelModel->level->vertices[std::get<0>(surf.vertices[1])] - levelModel->level->vertices[std::get<0>(surf.vertices[0])];

    auto sb0 = dnsb0 / length_squared(dnsb0);
    auto sb1 = cross(surf.normal, sb0);

    unsigned int noncol_vert;
    for(noncol_vert = 2; noncol_vert < surf.vertices.size(); ++noncol_vert) {
        auto sb2 = levelModel->level->vertices[std::get<0>(surf.vertices[noncol_vert])] - levelModel->level->vertices[std::get<0>(surf.vertices[0])];
        if(fabsf(dot(sb1, sb2)) > 0.0f) {
            break;
        }
    }

    auto vb0 = levelModel->level->texture_vertices[std::get<1>(surf.vertices[1])] - levelModel->level->texture_vertices[std::get<1>(surf.vertices[0])];
    auto vb1 = make_vector(get<1>(vb0), -get<0>(vb0));

    float sgn = 1.0f;
    if(dot(vb1, levelModel->level->texture_vertices[std::get<1>(surf.vertices[noncol_vert])] - levelModel->level->texture_vertices[std::get<1>(surf.vertices[0])]) < 0.0f) {
        sgn = -1.0f;
    }

    auto tb0 = -vb0;
    auto tb1 = -sgn * vb1;

    // Create animation
    thing_id emplaced_anim = levelModel->ecs.emplace_entity();

    levelModel->ecs.emplace_component<components::slide_surface>(emplaced_anim, surface, direction, sb0, sb1, tb0, tb1);
    levelModel->ecs.emplace_component<components::surface_animation>(emplaced_anim, surface);

    return emplaced_anim;
}

gorc::thing_id animation_presenter::slide_ceiling_sky(float u_speed, float v_speed) {
    thing_id anim = levelModel->ecs.emplace_entity();
    levelModel->ecs.emplace_component<components::slide_ceiling_sky>(anim, make_vector(u_speed, v_speed));
    return anim;
}

gorc::thing_id animation_presenter::surface_light_anim(surface_id surface, float start_light, float end_light, float change_time) {
    at_id(levelModel->surfaces, surface).extra_light = start_light;

    thing_id emplaced_anim = levelModel->ecs.emplace_entity();

    levelModel->ecs.emplace_component<components::surface_light>(emplaced_anim, surface, start_light, end_light, change_time);
    levelModel->ecs.emplace_component<components::surface_animation>(emplaced_anim, surface);

    return emplaced_anim;
}

void gorc::game::world::animations::animation_presenter::register_verbs(cog::verb_table &verbs, level_state &components) {
    verbs.add_verb("materialanim", [&components](int, float, int) {
        // TODO: Implement this verb when setsurfacematerial changes have been made.
        return -1;
    });

    verbs.add_safe_verb("getsurfaceanim", thing_id(invalid_id), [&components](surface_id surface) {
        return components.current_level_presenter->animation_presenter->get_surface_anim(surface);
    });

    verbs.add_safe_verb("stopsurfaceanim", cog::value(), [&components](surface_id surface) {
        components.current_level_presenter->animation_presenter->stop_surface_anim(surface);
    });

    verbs.add_safe_verb("getwallcel", -1, [&components](surface_id surface) {
        return components.current_level_presenter->animation_presenter->get_surface_cel(surface);
    });

    verbs.add_safe_verb("setwallcel", cog::value(), [&components](surface_id surface, int cel) {
        components.current_level_presenter->animation_presenter->set_surface_cel(surface, cel);
        return 1;
    });

    verbs.add_safe_verb("surfaceanim", thing_id(invalid_id), [&components](surface_id surface, float rate, int flags) {
        return components.current_level_presenter->animation_presenter->surface_anim(surface, rate, flag_set<flags::anim_flag>(flags));
    });

    verbs.add_verb("slideceilingsky", [&components](float u_speed, float v_speed) {
        return components.current_level_presenter->animation_presenter->slide_ceiling_sky(u_speed, v_speed);
    });

    verbs.add_safe_verb("slidesurface", thing_id(invalid_id), [&components](surface_id surface, vector<3> direction, float speed) {
        return components.current_level_presenter->animation_presenter->slide_surface(surface, normalize(direction) * speed);
    });

    verbs.add_safe_verb("slidewall", thing_id(invalid_id), [&components](surface_id surface, vector<3> direction, float speed) {
        return components.current_level_presenter->animation_presenter->slide_surface(surface, normalize(direction) * speed);
    });

    verbs.add_safe_verb("stopanim", cog::value(), [&components](thing_id anim) {
        components.current_level_presenter->animation_presenter->stop_anim(anim);
    });

    verbs.add_safe_verb("surfacelightanim", thing_id(invalid_id), [&components](surface_id surface, float start_light, float end_light, float change_time) {
        return components.current_level_presenter->animation_presenter->surface_light_anim(surface, start_light, end_light, change_time);
    });
}
