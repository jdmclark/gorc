#include "character_controller_aspect.h"
#include "game/world/level_presenter.h"
#include "game/world/level_model.h"
#include "game/constants.h"
#include "game/world/components/player.h"
#include "game/world/scripts/script_presenter.h"
#include "game/world/keys/key_presenter.h"
#include "game/world/physics/physics_presenter.h"
#include "game/world/events/thing_created.h"
#include "game/world/events/jumped.h"
#include "game/world/events/landed.h"
#include "game/world/events/standing_material_changed.h"
#include "game/world/events/killed.h"
#include <type_traits>

using gorc::game::world::aspects::character_controller_aspect;

gorc::flags::standing_material_type character_controller_aspect::get_standing_material(components::thing& thing) {
    if(thing.attach_flags & flags::attach_flag::AttachedToThingFace) {
        auto& floor_thing = presenter.model->get_thing(thing.attached_thing);
        if(floor_thing.flags & flags::thing_flag::Metal) {
            return flags::standing_material_type::metal;
        }
        else if(floor_thing.flags & flags::thing_flag::Dirt) {
            return flags::standing_material_type::dirt;
        }
        else {
            return flags::standing_material_type::hard;
        }
    }
    else if(thing.attach_flags & flags::attach_flag::AttachedToWorldSurface) {
        auto& floor_surf = presenter.model->surfaces[thing.attached_surface];

        if(floor_surf.flags & flags::surface_flag::Metal) {
            return flags::standing_material_type::metal;
        }
        else if(floor_surf.flags & flags::surface_flag::Dirt) {
            return flags::standing_material_type::dirt;
        }
        else if(floor_surf.flags & flags::surface_flag::ShallowWater) {
            return flags::standing_material_type::shallow_water;
        }
        else if(floor_surf.flags & flags::surface_flag::DeepWater) {
            return flags::standing_material_type::deep_water;
        }
        else if(floor_surf.flags & flags::surface_flag::VeryDeepWater) {
            return flags::standing_material_type::very_deep_water;
        }
        else {
            return flags::standing_material_type::hard;
        }
    }
    else {
        return flags::standing_material_type::none;
    }
}

bool character_controller_aspect::can_stand_on_thing(int surface_thing_id) {
    auto const &surface_thing = presenter.model->get_thing(surface_thing_id);
    return (surface_thing.collide == flags::collide_type::face) &&
           (surface_thing.flags & flags::thing_flag::CanStandOn);
}

bool character_controller_aspect::can_stand_on_surface(int surface_id) {
    auto const &surface = presenter.model->surfaces[surface_id];
    return surface.flags & flags::surface_flag::Floor;
}

gorc::maybe<gorc::game::world::physics::contact> character_controller_aspect::run_falling_sweep(entity_id thing_id, components::thing& thing,
                double) {
    // Test for collision between legs and ground using multiple tests
    auto leg_height = thing.model_3d->insert_offset;

    if(thing.physics_flags & flags::physics_flag::is_crouching) {
        leg_height = normalize(leg_height) * thing.size;
    }

    maybe<physics::contact> contact;

    contact = presenter.physics_presenter->thing_segment_query(thing_id, -leg_height,
            [&](int t) { return can_stand_on_thing(t); },
            [&](int s) { return can_stand_on_surface(s); },
            contact);
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

gorc::maybe<gorc::game::world::physics::contact> character_controller_aspect::run_walking_sweep(entity_id thing_id, components::thing& thing,
        double) {
    // Test for collision between legs and ground using multiple tests
    auto leg_height = thing.model_3d->insert_offset * 1.50f;

    maybe<physics::contact> contact;

    contact = presenter.physics_presenter->thing_segment_query(thing_id, -leg_height,
            [&](int t) { return can_stand_on_thing(t); },
            [&](int s) { return can_stand_on_surface(s); },
            contact);
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

void character_controller_aspect::update_falling(entity_id thing_id, components::thing& thing, double dt) {
    auto maybe_contact = run_falling_sweep(thing_id, thing, dt);

    auto applied_thrust = thing.thrust;
    get<2>(applied_thrust) = 0.0f;
    thing.vel = thing.vel + applied_thrust * static_cast<float>(dt);

    if(physics::contact const* contact = maybe_contact) {
        // Check if attached surface/thing has changed.
        if(int const* attachment_id = contact->contact_surface_id) {
            land_on_surface(thing_id, thing, *attachment_id, *contact);
        }

        if(int const* attachment_id = contact->contact_thing_id) {
            land_on_thing(thing_id, thing, *attachment_id, *contact);
        }
    }
}

void character_controller_aspect::update_standing(entity_id thing_id, components::thing& thing, double dt) {
    auto maybe_contact = run_walking_sweep(thing_id, thing, dt);
    auto oriented_vel = invert(thing.orient).transform(thing.vel);
    auto vel_fb = get<1>(oriented_vel);
    auto vel_lr = get<0>(oriented_vel);

    if(const physics::contact* contact = maybe_contact) {
        // Check if attached surface/thing has changed.
        if(const int* attachment_id = contact->contact_surface_id) {
            if(!(thing.attach_flags & flags::attach_flag::AttachedToWorldSurface) || *attachment_id != thing.attached_surface) {
                // Player has landed on a new surface.
                step_on_surface(thing_id, thing, *attachment_id, *contact);
            }
        }

        if(const int* attachment_id = contact->contact_thing_id) {
            if(!(thing.attach_flags & flags::attach_flag::AttachedToThingFace) || *attachment_id != thing.attached_thing) {
                // Player has landed on a new thing.
                step_on_thing(thing_id, thing, *attachment_id, *contact);
            }
        }

        //Adjust thrust depending on action
        if(thing.physics_flags & flags::physics_flag::is_crouching) {
            thing.thrust *= ((float)4 / (float)15);
        }
        else {
            if(fabs(vel_lr) > fabs(vel_fb)) {
                thing.thrust *= 0.5f;
            }
            else {
                thing.thrust *= ((float)2 / (float)3);
            }

            if(thing.physics_flags & flags::physics_flag::is_running) {
                thing.thrust *= 2;
            }
        }

        //Walking backwards
        if(vel_fb < 0.0f && fabs(vel_lr) < fabs(vel_fb)) {
            thing.thrust *= 0.5f;
        }

        if(get<2>(thing.thrust) > 0.0f) {
            jump(thing_id, thing);
        }
        else {
            // Accelerate body along surface
            auto hit_world = contact->position;
            auto hit_normal = contact->normal;
            auto player_new_vel = thing.thrust - hit_normal * dot(thing.thrust, hit_normal);
            auto new_vel = player_new_vel;

            if(const int* attachment_id = contact->contact_surface_id) {
                new_vel += presenter.model->surfaces[*attachment_id].thrust;
            }

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
        }
    }
    else {
        set_is_falling(thing_id, thing);
    }
}

void character_controller_aspect::set_is_falling(entity_id thing_id, components::thing& thing) {
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

    cs.bus.fire_event(events::standing_material_changed(thing_id, flags::standing_material_type::none));
}

bool character_controller_aspect::step_on_surface(entity_id thing_id, components::thing& thing, unsigned int surf_id,
                const physics::contact&) {
    const auto& surface = presenter.model->surfaces[surf_id];
    if(surface.flags & flags::surface_flag::Floor) {
        thing.attach_flags = flag_set<flags::attach_flag> { flags::attach_flag::AttachedToWorldSurface };
        thing.attached_surface = surf_id;

        presenter.script_presenter->send_message_to_linked(cog::message_id::entered,
                surf_id, flags::message_type::surface,
                thing_id, flags::message_type::thing);

        cs.bus.fire_event(events::standing_material_changed(thing_id, get_standing_material(thing)));
        return true;
    }
    else {
        set_is_falling(thing_id, thing);
        return false;
    }

    // Player has landed.
    //presenter.AdjustThingPosition(thing_id, Math::VecBt(rrcb.m_hitPointWorld) + thing.Model3d->InsertOffset);
}

bool character_controller_aspect::step_on_thing(entity_id thing_id, components::thing& thing, int land_thing_id,
                const physics::contact&) {
    const auto& attach_thing = presenter.model->get_thing(land_thing_id);
    if(attach_thing.flags & flags::thing_flag::CanStandOn) {
        thing.attach_flags = flag_set<flags::attach_flag> { flags::attach_flag::AttachedToThingFace };
        thing.attached_thing = land_thing_id;
        thing.prev_attached_thing_position = presenter.model->get_thing(land_thing_id).position;

        presenter.script_presenter->send_message_to_linked(cog::message_id::entered,
                land_thing_id, flags::message_type::thing,
                thing_id, flags::message_type::thing);

        cs.bus.fire_event(events::standing_material_changed(thing_id, get_standing_material(thing)));
        return true;
    }
    else {
        set_is_falling(thing_id, thing);
        return false;
    }

    // Player has landed.
    //presenter.AdjustThingPosition(thing_id, Math::VecBt(rrcb.m_hitPointWorld) + thing.Model3d->InsertOffset);
}

void character_controller_aspect::land_on_surface(entity_id thing_id,
                                                  components::thing &thing,
                                                  unsigned int surf_id,
                                                  physics::contact const &rrcb) {
    if(!step_on_surface(thing_id, thing, surf_id, rrcb)) {
        return;
    }

    cs.bus.fire_event(events::landed(thing_id));
}

void character_controller_aspect::land_on_thing(entity_id thing_id, components::thing& thing, int land_thing_id,
                const physics::contact& rrcb) {
    if(!step_on_thing(thing_id, thing, land_thing_id, rrcb)) {
        return;
    }

    cs.bus.fire_event(events::landed(thing_id));
}

void character_controller_aspect::jump(entity_id thing_id, components::thing& thing) {
    cs.bus.fire_event(events::jumped(thing_id));
    set_is_falling(thing_id, thing);
    thing.vel = thing.vel + make_vector(0.0f, 0.0f, get<2>(thing.thrust));
}

void character_controller_aspect::update(time t,
                                         entity_id thing_id,
                                         components::character&,
                                         components::thing &thing) {
    // Update actor state
    if(static_cast<int>(thing.attach_flags)) {
        update_standing(thing_id, thing, t.elapsed_as_seconds());
    }
    else {
        update_falling(thing_id, thing, t.elapsed_as_seconds());
    }

    // Update lightsaber state
    if(thing.jk_flags & flags::jk_flag::has_saber) {
        if(thing.jk_flags & flags::jk_flag::saber_igniting) {
            thing.saber_drawn_length += static_cast<float>(fabs(thing.saber_drawn_length - thing.saber_length) *
            t.elapsed_as_seconds() * 6.0);
            if(thing.saber_drawn_length >= thing.saber_length) {
                thing.saber_drawn_length = thing.saber_length;
                thing.jk_flags -= flags::jk_flag::saber_igniting;
            }
        }
        else if(thing.jk_flags & flags::jk_flag::saber_shrinking) {
            thing.saber_drawn_length -= thing.saber_drawn_length * static_cast<float>(t.elapsed_as_seconds()) * 6.0f;
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

void character_controller_aspect::on_killed(entity_id thing_id,
                                            components::thing &thing,
                                            entity_id killer) {
    presenter.script_presenter->send_message_to_linked(cog::message_id::killed,
                                                       static_cast<int>(thing_id),
                                                       flags::message_type::thing,
                                                       killer,
                                                       flags::message_type::thing);

    thing.type = flags::thing_type::Corpse;
}

character_controller_aspect::character_controller_aspect(component_system &cs,
                                                         level_presenter &presenter)
    : inner_join_aspect(cs), presenter(presenter) {

    cs.bus.add_handler<events::thing_created>([&](events::thing_created const &e) {
        if(e.tpl.type == flags::thing_type::Actor ||
           e.tpl.type == flags::thing_type::Player ) {
            cs.emplace_component<components::character>(e.thing);
        }

        if(e.tpl.type == flags::thing_type::Player) {
            // TODO: When player gets an aspect, this should be moved there.
            cs.emplace_component<components::player>(e.thing);
        }
    });

    cs.bus.add_handler<events::killed>([&](events::killed const &e) {
        for(auto &ch : cs.find_component<components::character>(e.thing)) {
            for(auto &thing : cs.find_component<components::thing>(e.thing)) {
                on_killed(e.thing, thing.second, e.killer);
            }
        }
    });

    return;
}
