#include "character_controller.h"
#include "game/world/level_presenter.h"
#include "game/world/level_model.h"
#include "game/constants.h"
#include "game/world/scripts/script_presenter.h"
#include "game/world/keys/key_presenter.h"
#include "game/world/sounds/sound_presenter.h"
#include "game/world/physics/physics_presenter.h"
#include <unordered_map>
#include <type_traits>

namespace gorc {
namespace game {
namespace world {
namespace gameplay {

/* none,
    hard,
    dirt,
    metal,
    shallow_water,
    deep_water,
    very_deep_water*/

template <typename T> class enum_hasher {
    std::hash<typename std::underlying_type<T>::type> hasher;

public:
    size_t operator()(T value) const {
        return hasher(static_cast<typename std::underlying_type<T>::type>(value));
    }
};

const std::unordered_map<standing_material, flags::sound_subclass_type, enum_hasher<standing_material>> character_left_run_map {
    { standing_material::none, flags::sound_subclass_type::LRunHard },
    { standing_material::hard, flags::sound_subclass_type::LRunHard },
    { standing_material::dirt, flags::sound_subclass_type::LRunEarth },
    { standing_material::metal, flags::sound_subclass_type::LRunMetal },
    { standing_material::shallow_water, flags::sound_subclass_type::LRunPuddle },
    { standing_material::deep_water, flags::sound_subclass_type::LRunWater },
    { standing_material::very_deep_water, flags::sound_subclass_type::LRunWater },
};

const std::unordered_map<standing_material, flags::sound_subclass_type, enum_hasher<standing_material>> character_right_run_map {
    { standing_material::none, flags::sound_subclass_type::RRunHard },
    { standing_material::hard, flags::sound_subclass_type::RRunHard },
    { standing_material::dirt, flags::sound_subclass_type::RRunEarth },
    { standing_material::metal, flags::sound_subclass_type::RRunMetal },
    { standing_material::shallow_water, flags::sound_subclass_type::RRunPuddle },
    { standing_material::deep_water, flags::sound_subclass_type::RRunWater },
    { standing_material::very_deep_water, flags::sound_subclass_type::RRunWater },
};

const std::unordered_map<standing_material, flags::sound_subclass_type, enum_hasher<standing_material>> character_left_walk_map {
    { standing_material::none, flags::sound_subclass_type::LWalkHard },
    { standing_material::hard, flags::sound_subclass_type::LWalkHard },
    { standing_material::dirt, flags::sound_subclass_type::LWalkEarth },
    { standing_material::metal, flags::sound_subclass_type::LWalkMetal },
    { standing_material::shallow_water, flags::sound_subclass_type::LWalkPuddle },
    { standing_material::deep_water, flags::sound_subclass_type::LWalkWater },
    { standing_material::very_deep_water, flags::sound_subclass_type::LWalkWater },
};

const std::unordered_map<standing_material, flags::sound_subclass_type, enum_hasher<standing_material>> character_right_walk_map {
    { standing_material::none, flags::sound_subclass_type::RWalkHard },
    { standing_material::hard, flags::sound_subclass_type::RWalkHard },
    { standing_material::dirt, flags::sound_subclass_type::RWalkEarth },
    { standing_material::metal, flags::sound_subclass_type::RWalkMetal },
    { standing_material::shallow_water, flags::sound_subclass_type::RWalkPuddle },
    { standing_material::deep_water, flags::sound_subclass_type::RWalkWater },
    { standing_material::very_deep_water, flags::sound_subclass_type::RWalkWater },
};

template <typename T, typename U> auto map_get(const T& map, U key) -> decltype(map.find(key)->second) {
    auto it = map.find(key);
    assert(it != map.end());
    return it->second;
}

}
}
}
}

gorc::flags::puppet_mode_type gorc::game::world::gameplay::character_controller::get_puppet_mode(thing& thing) {
    bool is_underwater = (presenter.model->sectors[thing.sector].flags & flags::sector_flag::Underwater);

    switch(thing.armed_mode) {
    default:
    case flags::armed_mode::unarmed:
        return is_underwater? flags::puppet_mode_type::swimming_unarmed : flags::puppet_mode_type::unarmed;

    case flags::armed_mode::armed:
        return is_underwater? flags::puppet_mode_type::swimming_armed : flags::puppet_mode_type::armed;

    case flags::armed_mode::saber:
        return is_underwater? flags::puppet_mode_type::swimming_saber : flags::puppet_mode_type::saber;
    }
}

void gorc::game::world::gameplay::character_controller::set_walk_animation(thing& thing, flags::puppet_submode_type type, float speed) {
    if(thing.actor_walk_animation >= 0) {
        keys::key_state& keyState = presenter.model->key_model.keys[thing.actor_walk_animation];
        const content::assets::puppet_submode& submode = thing.pup->get_mode(thing.puppet_mode).get_submode(type);
        if(!keyState.animation) {
            return;
        }

        if(keyState.animation != submode.anim) {
            keyState.animation_time = 0.0;
        }

        keyState.animation = submode.anim;
        keyState.high_priority = submode.hi_priority;
        keyState.low_priority = submode.lo_priority;
        keyState.flags = flag_set<flags::key_flag>();
        keyState.speed = speed;
    }
}

bool gorc::game::world::gameplay::character_controller::is_walk_animation_mode(thing& thing, flags::puppet_submode_type type) {
    if(thing.actor_walk_animation >= 0) {
        keys::key_state& keyState = presenter.model->key_model.keys[thing.actor_walk_animation];
        const content::assets::puppet_submode& submode = thing.pup->get_mode(thing.puppet_mode).get_submode(type);
        return keyState.animation == submode.anim;
    }

    return false;
}

void gorc::game::world::gameplay::character_controller::set_walk_animation_speed(thing& thing, float speed) {
    if(thing.actor_walk_animation >= 0) {
        keys::key_state& keyState = presenter.model->key_model.keys[thing.actor_walk_animation];
        keyState.speed = speed;
    }
}

void gorc::game::world::gameplay::character_controller::play_falling_animation(int thing_id, thing& thing) {
    if(dot(thing.vel, make_vector(0.0f, 0.0f, -1.0f))) {
        // Player's trajectory has reached apogee.
        set_walk_animation(thing, flags::puppet_submode_type::Drop, 1.0f);
    }
}

void gorc::game::world::gameplay::character_controller::play_standing_animation(int thing_id, thing& thing) {
    auto oriented_vel = invert(thing.orient).transform(thing.vel);
    auto run_length = length(thing.vel);

    auto vel_fb = get<1>(oriented_vel);
    auto vel_lr = get<0>(oriented_vel);
    auto turn_rate = get<1>(thing.ang_vel);

    float run_anim_speed = run_length * 20.0f;
    float turn_anim_speed = fabs(turn_rate) / 360.0f;

    if(thing.physics_flags & flags::physics_flag::is_crouching) {
        if(fabs(turn_rate) > 0.0001f) {
            set_walk_animation(thing, flags::puppet_submode_type::CrouchForward, turn_anim_speed * 20.0f);
        }
        else if(vel_fb >= 0.0f || fabs(vel_lr) > fabs(vel_fb)) {
            set_walk_animation(thing, flags::puppet_submode_type::CrouchForward, run_anim_speed);
        }
        else {
            set_walk_animation(thing, flags::puppet_submode_type::CrouchBack, run_anim_speed);
        }
    }
    else {
        if(run_length < 0.001f) {
            // Idle or turning.
            if(turn_rate > 60.0f) {
                // Turning right
                set_walk_animation(thing, flags::puppet_submode_type::TurnRight, turn_anim_speed);
            }
            else if(turn_rate < -60.0f) {
                // Turning left
                set_walk_animation(thing, flags::puppet_submode_type::TurnLeft, turn_anim_speed);
            }
            else if(fabs(turn_rate) < 0.001f) {
                set_walk_animation(thing, flags::puppet_submode_type::Stand, 1.0f);
            }
            else if(!is_walk_animation_mode(thing, flags::puppet_submode_type::Stand)) {
                set_walk_animation_speed(thing, turn_anim_speed);
            }
        }
        else if(fabs(vel_lr) > fabs(vel_fb)) {
            // Strafing left or right
            if(vel_lr >= 0.0f) {
                set_walk_animation(thing, flags::puppet_submode_type::StrafeRight, run_anim_speed);
            }
            else {
                set_walk_animation(thing, flags::puppet_submode_type::StrafeLeft, run_anim_speed);
            }
        }
        else if(vel_fb > 0.5f) {
            // Running forward
            set_walk_animation(thing, flags::puppet_submode_type::Run, run_anim_speed);
        }
        else if(vel_fb > 0.0f) {
            set_walk_animation(thing, flags::puppet_submode_type::Walk, run_anim_speed);
        }
        else {
            // Walking backwards
            set_walk_animation(thing, flags::puppet_submode_type::WalkBack, run_anim_speed);
        }
    }
}

gorc::game::world::gameplay::standing_material gorc::game::world::gameplay::character_controller::get_standing_material(thing& thing) {
    if(thing.attach_flags & flags::attach_flag::AttachedToThingFace) {
        auto& floor_thing = presenter.model->things[thing.attached_thing];
        if(floor_thing.flags & flags::thing_flag::Metal) {
            return standing_material::metal;
        }
        else if(floor_thing.flags & flags::thing_flag::Dirt) {
            return standing_material::dirt;
        }
        else {
            return standing_material::hard;
        }
    }
    else if(thing.attach_flags & flags::attach_flag::AttachedToWorldSurface) {
        auto& floor_surf = presenter.model->surfaces[thing.attached_surface];

        if(floor_surf.flags & flags::surface_flag::Metal) {
            return standing_material::metal;
        }
        else if(floor_surf.flags & flags::surface_flag::Dirt) {
            return standing_material::dirt;
        }
        else if(floor_surf.flags & flags::surface_flag::ShallowWater) {
            return standing_material::shallow_water;
        }
        else if(floor_surf.flags & flags::surface_flag::DeepWater) {
            return standing_material::deep_water;
        }
        else if(floor_surf.flags & flags::surface_flag::VeryDeepWater) {
            return standing_material::very_deep_water;
        }
        else {
            return standing_material::hard;
        }
    }
    else {
        return standing_material::none;
    }
}

gorc::maybe<gorc::game::world::physics::contact> gorc::game::world::gameplay::character_controller::run_falling_sweep(int thing_id, thing& thing,
                double dt) {
    // Test for collision between legs and ground using multiple tests
    auto leg_height = thing.model_3d->insert_offset;

    if(thing.physics_flags & flags::physics_flag::is_crouching) {
        leg_height = normalize(leg_height) * thing.size;
    }

    maybe<physics::contact> contact;

    contact = presenter.physics_presenter->thing_segment_query(thing_id, -leg_height,
            [&](int t) { return presenter.physics_presenter->thing_needs_collision_response(thing_id, t); },
            [&](int s) { return presenter.physics_presenter->surface_needs_collision_response(thing_id, s); }, contact);
    if(contact) {
        return contact;
    }

    // TODO: Revisit character controller falling sweep.
    /*for(int i = 0; i < 8; ++i) {
        float a = static_cast<float>(i) * 0.7853981633974483f;
        auto leg_offset = (-leg_height) + make_vector<float>(-sin(a), cos(a), 0.0f) * thing.size;
        contact = presenter.physics_presenter.thing_segment_query(thing_id, leg_offset, contact);
    }*/

    return contact;
}

gorc::maybe<gorc::game::world::physics::contact> gorc::game::world::gameplay::character_controller::run_walking_sweep(int thing_id, thing& thing,
        double dt) {
    // Test for collision between legs and ground using multiple tests
    auto leg_height = thing.model_3d->insert_offset * 1.50f;

    maybe<physics::contact> contact;

    contact = presenter.physics_presenter->thing_segment_query(thing_id, -leg_height,
            [&](int t) { return presenter.physics_presenter->thing_needs_collision_response(thing_id, t); },
            [&](int s) { return presenter.physics_presenter->surface_needs_collision_response(thing_id, s); }, contact);
    if(contact) {
        return contact;
    }

    // TODO: Revisit character controller walking sweep.
    /*for(int i = 0; i < 8; ++i) {
        float a = static_cast<float>(i) * 0.7853981633974483f;
        auto leg_offset = (-leg_height) + make_vector<float>(-sin(a), cos(a), 0.0f) * thing.size;
        contact = presenter.physics_presenter.thing_segment_query(thing_id, leg_offset, contact);
    }*/

    return contact;
}

void gorc::game::world::gameplay::character_controller::update_falling(int thing_id, thing& thing, double dt) {
    auto maybe_contact = run_falling_sweep(thing_id, thing, dt);

    auto applied_thrust = thing.thrust;
    get<2>(applied_thrust) = 0.0f;
    thing.vel = thing.vel + applied_thrust * dt;

    play_falling_animation(thing_id, thing);

    if_set(maybe_contact, then_do, [this, &thing, thing_id](const physics::contact& contact) {
        // Check if attached surface/thing has changed.
        if_set(contact.contact_surface_id, then_do, [this, thing_id, &thing, &contact](int attachment_id) {
            land_on_surface(thing_id, thing, attachment_id, contact);
        });

        if_set(contact.contact_thing_id, then_do, [this, &thing, thing_id, &contact](int attachment_id) {
            land_on_thing(thing_id, thing, attachment_id, contact);
        });
    });
}

void gorc::game::world::gameplay::character_controller::update_standing(int thing_id, thing& thing, double dt) {
    auto maybe_contact = run_walking_sweep(thing_id, thing, dt);

    if_set(maybe_contact, then_do, [this, &thing, thing_id, dt](const physics::contact& contact) {
        // Check if attached surface/thing has changed.
        if_set(contact.contact_surface_id, then_do, [this, thing_id, &thing, &contact](int attachment_id) {
            if(!(thing.attach_flags & flags::attach_flag::AttachedToWorldSurface) || attachment_id != thing.attached_surface) {
                // Player has landed on a new surface.
                step_on_surface(thing_id, thing, attachment_id, contact);
            }
        });

        if_set(contact.contact_thing_id, then_do, [this, &thing, thing_id, &contact](int attachment_id) {
            if(!(thing.attach_flags & flags::attach_flag::AttachedToThingFace) || attachment_id != thing.attached_thing) {
                // Player has landed on a new thing.
                step_on_thing(thing_id, thing, attachment_id, contact);
            }
        });

        if(get<2>(thing.thrust) > 0.0f) {
            jump(thing_id, thing);
        }
        else {
            // Accelerate body along surface
            auto hit_world = contact.position;
            auto hit_normal = contact.normal;
            auto player_new_vel = thing.thrust - hit_normal * dot(thing.thrust, hit_normal);
            auto new_vel = player_new_vel;

            if_set(contact.contact_surface_id, then_do, [&new_vel, this](int attachment_id) {
                new_vel += presenter.model->surfaces[attachment_id].thrust;
            });

            // Accelerate body toward standing position
            if(thing.physics_flags & flags::physics_flag::is_crouching) {
                float dist = dot(hit_normal, thing.position - hit_world);
                auto io_nrm = normalize(thing.model_3d->insert_offset);
                auto hover_position = hit_world + io_nrm * (thing.size + 0.01f);
                new_vel += (hover_position - thing.position) * 20.0f;
            }
            else {
                float dist = dot(hit_normal, thing.position - hit_world);
                auto hover_position = hit_world + thing.model_3d->insert_offset;
                new_vel += (hover_position - thing.position) * 20.0f;
            }

            thing.vel = new_vel;

            // Update idle animation
            play_standing_animation(thing_id, thing);
        }
    },
    else_do, [&thing, thing_id, this] {
        set_is_falling(thing_id, thing);
    });
}

void gorc::game::world::gameplay::character_controller::set_is_falling(int thing_id, thing& thing) {
    // Player is falling again.
    if(thing.attach_flags & flags::attach_flag::AttachedToThingFace) {
        presenter.script_presenter->send_message_to_linked(cog::message_id::exited,
                thing.attached_thing, flags::message_type::thing,
                thing_id, flags::message_type::thing);
    }
    else if(thing.attach_flags & flags::attach_flag::AttachedToWorldSurface) {
        presenter.script_presenter->send_message_to_linked(cog::message_id::exited,
                thing.attached_surface, flags::message_type::surface,
                thing_id, flags::message_type::thing);
    }

    thing.attach_flags = flag_set<flags::attach_flag>();
}

bool gorc::game::world::gameplay::character_controller::step_on_surface(int thing_id, thing& thing, unsigned int surf_id,
                const physics::contact& rrcb) {
    const auto& surface = presenter.model->surfaces[surf_id];
    if(surface.flags & flags::surface_flag::Floor) {
        thing.attach_flags = flag_set<flags::attach_flag> { flags::attach_flag::AttachedToWorldSurface };
        thing.attached_surface = surf_id;

        presenter.script_presenter->send_message_to_linked(cog::message_id::entered,
                surf_id, flags::message_type::surface,
                thing_id, flags::message_type::thing);
        return true;
    }
    else {
        set_is_falling(thing_id, thing);
        return false;
    }

    // Player has landed.
    //presenter.AdjustThingPosition(thing_id, Math::VecBt(rrcb.m_hitPointWorld) + thing.Model3d->InsertOffset);
}

bool gorc::game::world::gameplay::character_controller::step_on_thing(int thing_id, thing& thing, int land_thing_id,
                const physics::contact& rrcb) {
    const auto& attach_thing = presenter.model->things[land_thing_id];
    if(attach_thing.flags & flags::thing_flag::CanStandOn) {
        thing.attach_flags = flag_set<flags::attach_flag> { flags::attach_flag::AttachedToThingFace };
        thing.attached_thing = land_thing_id;
        thing.prev_attached_thing_position = presenter.model->things[land_thing_id].position;

        presenter.script_presenter->send_message_to_linked(cog::message_id::entered,
                land_thing_id, flags::message_type::thing,
                thing_id, flags::message_type::thing);
        return true;
    }
    else {
        set_is_falling(thing_id, thing);
        return false;
    }

    // Player has landed.
    //presenter.AdjustThingPosition(thing_id, Math::VecBt(rrcb.m_hitPointWorld) + thing.Model3d->InsertOffset);
}

void gorc::game::world::gameplay::character_controller::land_on_surface(int thing_id, thing& thing, unsigned int surf_id,
                const physics::contact& rrcb) {
    if(!step_on_surface(thing_id, thing, surf_id, rrcb)) {
        return;
    }

    presenter.key_presenter->play_mode(thing_id, flags::puppet_submode_type::Land);

    const auto& surf = presenter.model->surfaces[surf_id];

    flags::sound_subclass_type subclass = flags::sound_subclass_type::LandHard;
    if(surf.flags & flags::surface_flag::Metal) {
        subclass = flags::sound_subclass_type::LandMetal;
    }
    else if(surf.flags & flags::surface_flag::Dirt) {
        subclass = flags::sound_subclass_type::LandEarth;
    }
    else if(surf.flags & flags::surface_flag::ShallowWater) {
        subclass = flags::sound_subclass_type::LandPuddle;
    }
    else if(surf.flags & flags::surface_flag::DeepWater) {
        subclass = flags::sound_subclass_type::LandWater;
    }
    else if(surf.flags & flags::surface_flag::VeryDeepWater) {
        subclass = flags::sound_subclass_type::LandWater;
    }

    presenter.sound_presenter->play_sound_class(thing_id, subclass);
}

void gorc::game::world::gameplay::character_controller::land_on_thing(int thing_id, thing& thing, int land_thing_id,
                const physics::contact& rrcb) {
    if(!step_on_thing(thing_id, thing, land_thing_id, rrcb)) {
        return;
    }

    presenter.key_presenter->play_mode(thing_id, flags::puppet_submode_type::Land);

    flag_set<flags::thing_flag> flags = presenter.model->things[land_thing_id].flags;

    flags::sound_subclass_type subclass = flags::sound_subclass_type::LandHard;
    if(flags & flags::thing_flag::Metal) {
        subclass = flags::sound_subclass_type::LandMetal;
    }
    else if(flags & flags::thing_flag::Dirt) {
        subclass = flags::sound_subclass_type::LandEarth;
    }

    presenter.sound_presenter->play_sound_class(thing_id, subclass);
}

void gorc::game::world::gameplay::character_controller::jump(int thing_id, thing& thing) {
    if(thing.attach_flags & flags::attach_flag::AttachedToWorldSurface) {
        jump_from_surface(thing_id, thing, thing.attached_surface);
    }
    else if(thing.attach_flags & flags::attach_flag::AttachedToThingFace) {
        jump_from_thing(thing_id, thing, thing.attached_thing);
    }

    if(thing.physics_flags & flags::physics_flag::is_crouching) {
        set_walk_animation(thing, flags::puppet_submode_type::Leap, 1.0f);
    }
    else {
        set_walk_animation(thing, flags::puppet_submode_type::Rising, 1.0f);
    }
}

void gorc::game::world::gameplay::character_controller::jump_from_surface(int thing_id, thing& thing, unsigned int surf_id) {
    set_is_falling(thing_id, thing);
    thing.vel = thing.vel + make_vector(0.0f, 0.0f, get<2>(thing.thrust));

    const auto& surf = presenter.model->surfaces[surf_id];

    flags::sound_subclass_type subclass = flags::sound_subclass_type::Jump;
    if(surf.flags & flags::surface_flag::Metal) {
        subclass = flags::sound_subclass_type::JumpMetal;
    }
    else if(surf.flags & flags::surface_flag::Dirt) {
        subclass = flags::sound_subclass_type::JumpEarth;
    }
    else if(surf.flags & flags::surface_flag::ShallowWater) {
        subclass = flags::sound_subclass_type::JumpWater;
    }
    else if(surf.flags & flags::surface_flag::DeepWater) {
        subclass = flags::sound_subclass_type::JumpWater;
    }
    else if(surf.flags & flags::surface_flag::VeryDeepWater) {
        subclass = flags::sound_subclass_type::JumpWater;
    }

    presenter.sound_presenter->play_sound_class(thing_id, subclass);
}

void gorc::game::world::gameplay::character_controller::jump_from_thing(int thing_id, thing& thing, int jump_thing_id) {
    set_is_falling(thing_id, thing);
    thing.vel = thing.vel + make_vector(0.0f, 0.0f, get<2>(thing.thrust));

    flag_set<flags::thing_flag> flags = presenter.model->things[jump_thing_id].flags;

    flags::sound_subclass_type subclass = flags::sound_subclass_type::Jump;
    if(flags & flags::thing_flag::Metal) {
        subclass = flags::sound_subclass_type::JumpMetal;
    }
    else if(flags & flags::thing_flag::Dirt) {
        subclass = flags::sound_subclass_type::JumpEarth;
    }

    presenter.sound_presenter->play_sound_class(thing_id, subclass);
}

void gorc::game::world::gameplay::character_controller::update(int thing_id, double dt) {
    thing_controller::update(thing_id, dt);

    thing& thing = presenter.model->things[thing_id];
    thing.puppet_mode = get_puppet_mode(thing);

    // Update actor state
    if(static_cast<int>(thing.attach_flags)) {
        update_standing(thing_id, thing, dt);
    }
    else {
        update_falling(thing_id, thing, dt);
    }

    // Update lightsaber state
    if(thing.jk_flags & flags::jk_flag::has_saber) {
        if(thing.jk_flags & flags::jk_flag::saber_igniting) {
            thing.saber_drawn_length += fabs(thing.saber_drawn_length - thing.saber_length) * dt * 6.0f;
            if(thing.saber_drawn_length >= thing.saber_length) {
                thing.saber_drawn_length = thing.saber_length;
                thing.jk_flags -= flags::jk_flag::saber_igniting;
            }
        }
        else if(thing.jk_flags & flags::jk_flag::saber_shrinking) {
            thing.saber_drawn_length -= thing.saber_drawn_length * dt * 6.0f;
            if(thing.saber_drawn_length <= 0.0f) {
                thing.saber_drawn_length = 0.0f;
                thing.jk_flags -= flag_set<flags::jk_flag> { flags::jk_flag::saber_shrinking, flags::jk_flag::has_saber };
            }
        }
        else {
            thing.saber_drawn_length = thing.saber_length;
        }
    }
    else {
        thing.jk_flags -= flag_set<flags::jk_flag> { flags::jk_flag::saber_igniting, flags::jk_flag::saber_shrinking };
        thing.saber_drawn_length = 0.0f;
    }
}

void gorc::game::world::gameplay::character_controller::create_controller_data(int thing_id) {
    thing_controller::create_controller_data(thing_id);

    auto& new_thing = presenter.model->things[thing_id];

    // HACK: Initialize actor walk animation
    if(new_thing.pup) {
        new_thing.actor_walk_animation = presenter.key_presenter->play_mode(thing_id, flags::puppet_submode_type::Stand);
    }
    else {
        new_thing.actor_walk_animation = -1;
    }

    if(new_thing.actor_walk_animation >= 0) {
        keys::key_state& keyState = presenter.model->key_model.keys[new_thing.actor_walk_animation];
        keyState.flags = flag_set<flags::key_flag>();
    }
}

void gorc::game::world::gameplay::character_controller::remove_controller_data(int thing_id) {
    auto& thing = presenter.model->things[thing_id];
    if(thing.actor_walk_animation >= 0) {
        presenter.key_presenter->stop_key(thing_id, thing.actor_walk_animation, 0.0f);
    }

    thing_controller::remove_controller_data(thing_id);
}

void gorc::game::world::gameplay::character_controller::handle_animation_marker(int thing_id, flags::key_marker_type marker) {
    auto& thing = presenter.model->things[thing_id];

    switch(marker) {
    case flags::key_marker_type::LeftRunFootstep:
        presenter.sound_presenter->play_sound_class(thing_id, map_get(character_left_run_map, get_standing_material(thing)));
        break;

    case flags::key_marker_type::RightRunFootstep:
        presenter.sound_presenter->play_sound_class(thing_id, map_get(character_right_run_map, get_standing_material(thing)));
        break;

    case flags::key_marker_type::LeftFootstep:
        presenter.sound_presenter->play_sound_class(thing_id, map_get(character_left_walk_map, get_standing_material(thing)));
        break;

    case flags::key_marker_type::RightFootstep:
        presenter.sound_presenter->play_sound_class(thing_id, map_get(character_right_walk_map, get_standing_material(thing)));
        break;
    }
}
