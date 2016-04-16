#include "character_controller_aspect.hpp"
#include "game/world/level_presenter.hpp"
#include "game/world/level_model.hpp"
#include "game/constants.hpp"
#include "game/world/components/player.hpp"
#include "game/world/keys/key_presenter.hpp"
#include "game/world/physics/physics_presenter.hpp"
#include "game/world/events/thing_created.hpp"
#include "game/world/events/jumped.hpp"
#include "game/world/events/landed.hpp"
#include "game/world/events/standing_material_changed.hpp"
#include "game/world/events/killed.hpp"
#include <type_traits>

using gorc::game::world::aspects::character_controller_aspect;

gorc::flags::standing_material_type character_controller_aspect::get_standing_material(components::thing& thing) {
    if(thing.attach_flags & flags::attach_flag::AttachedToThingFace) {
        auto& floor_thing = presenter.model->get_thing(thing.attached_thing.get_value());
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
        auto& floor_surf = at_id(presenter.model->surfaces, thing.attached_surface.get_value());

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

bool character_controller_aspect::can_stand_on_thing(thing_id surface_thing_id) {
    auto const &surface_thing = presenter.model->get_thing(surface_thing_id);
    return (surface_thing.collide == flags::collide_type::face) &&
           (surface_thing.flags & flags::thing_flag::CanStandOn);
}

bool character_controller_aspect::can_stand_on_surface(surface_id sid) {
    auto const &surface = at_id(presenter.model->surfaces, sid);
    return surface.flags & flags::surface_flag::Floor;
}

gorc::maybe<gorc::game::world::physics::contact> character_controller_aspect::run_falling_sweep(thing_id tid, components::thing& thing,
                double) {
    // Test for collision between legs and ground using multiple tests
    vector<3> leg_height, leg_height_norm;
    maybe_if(thing.model_3d, [&](auto model) {
        leg_height = model->insert_offset;
        auto len = length(leg_height);
        if(len > 0.0f) {
            leg_height_norm = leg_height / len;
        }
    });

    if(thing.physics_flags & flags::physics_flag::is_crouching) {
        leg_height = leg_height_norm * thing.size;
    }

    maybe<physics::contact> contact;

    contact = presenter.physics_presenter->thing_segment_query(tid, -leg_height,
            [&](thing_id t) { return can_stand_on_thing(t); },
            [&](surface_id s) { return can_stand_on_surface(s); },
            contact);

    // TODO: Revisit character controller falling sweep.
    /*for(int i = 0; i < 8; ++i) {
        float a = static_cast<float>(i) * 0.7853981633974483f;
        auto leg_offset = (-leg_height) + make_vector<float>(-sin(a), cos(a), 0.0f) * thing.size;
        contact = presenter.physics_presenter.thing_segment_query(thing_id, leg_offset, contact);
    }*/

    return contact;
}

gorc::maybe<gorc::game::world::physics::contact> character_controller_aspect::run_walking_sweep(thing_id tid, components::thing& thing,
        double) {
    // Test for collision between legs and ground using multiple tests
    vector<3> leg_height;
    maybe_if(thing.model_3d, [&](auto model) {
        leg_height = model->insert_offset * 1.50f;
    });

    maybe<physics::contact> contact;

    contact = presenter.physics_presenter->thing_segment_query(tid, -leg_height,
            [&](thing_id t) { return can_stand_on_thing(t); },
            [&](surface_id s) { return can_stand_on_surface(s); },
            contact);

    // TODO: Revisit character controller walking sweep.
    /*for(int i = 0; i < 8; ++i) {
        float a = static_cast<float>(i) * 0.7853981633974483f;
        auto leg_offset = (-leg_height) + make_vector<float>(-sin(a), cos(a), 0.0f) * thing.size;
        contact = presenter.physics_presenter.thing_segment_query(thing_id, leg_offset, contact);
    }*/

    return contact;
}

void character_controller_aspect::update_falling(thing_id tid, components::thing& thing, double dt) {
    auto maybe_contact = run_falling_sweep(tid, thing, dt);

    auto applied_thrust = thing.thrust;
    get<2>(applied_thrust) = 0.0f;
    thing.vel = thing.vel + applied_thrust * static_cast<float>(dt);

    maybe_if(maybe_contact, [&](physics::contact const &contact) {
        // Check if attached surface/thing has changed.
        maybe_if(contact.contact_surface_id, [&](surface_id attachment_id) {
            land_on_surface(tid, thing, attachment_id, contact);
        });

        maybe_if(contact.contact_thing_id, [&](thing_id attachment_id) {
            land_on_thing(tid, thing, attachment_id, contact);
        });
    });
}

void character_controller_aspect::update_standing(thing_id tid, components::thing& thing, double dt) {
    auto maybe_contact = run_walking_sweep(tid, thing, dt);

    if(maybe_contact.has_value()) {
        physics::contact const &contact = maybe_contact.get_value();

        // Check if attached surface/thing has changed.
        maybe_if(contact.contact_surface_id, [&](surface_id attachment_id) {
            if(!(thing.attach_flags & flags::attach_flag::AttachedToWorldSurface) || attachment_id != thing.attached_surface) {
                // Player has landed on a new surface.
                step_on_surface(tid, thing, attachment_id, contact);
            }
        });

        maybe_if(contact.contact_thing_id, [&](thing_id attachment_id) {
            if(!(thing.attach_flags & flags::attach_flag::AttachedToThingFace) || attachment_id != thing.attached_thing) {
                // Player has landed on a new thing.
                step_on_thing(tid, thing, attachment_id, contact);
            }
        });

        if(get<2>(thing.thrust) > 0.0f) {
            jump(tid, thing);
        }
        else {
            // Accelerate body along surface
            auto hit_world = contact.position;
            auto hit_normal = contact.normal;
            auto player_new_vel = thing.thrust - hit_normal * dot(thing.thrust, hit_normal);
            auto new_vel = player_new_vel;

            maybe_if(contact.contact_surface_id, [&](surface_id attachment_id) {
                new_vel += at_id(presenter.model->surfaces, attachment_id).thrust;
            });

            // Accelerate body toward standing position
            vector<3> insert_offset, insert_offset_norm;
            maybe_if(thing.model_3d, [&](auto model) {
                insert_offset = model->insert_offset;
                auto len = length(insert_offset);
                if(len > 0.0f) {
                    insert_offset_norm = insert_offset / len;
                }
            });

            if(thing.physics_flags & flags::physics_flag::is_crouching) {
                //float dist = dot(hit_normal, thing.position - hit_world);
                auto hover_position = hit_world + insert_offset_norm * (thing.size + 0.01f);
                new_vel += (hover_position - thing.position) * 20.0f;
            }
            else {
                //float dist = dot(hit_normal, thing.position - hit_world);
                auto hover_position = hit_world + insert_offset;
                new_vel += (hover_position - thing.position) * 20.0f;
            }

            thing.vel = new_vel;
        }
    }
    else {
        set_is_falling(tid, thing);
    }
}

void character_controller_aspect::set_is_falling(thing_id tid, components::thing& thing) {
    // Player is falling again.
    if(thing.attach_flags & flags::attach_flag::AttachedToThingFace) {
        maybe_if(thing.attached_thing, [&](thing_id attached_thing) {
                presenter.model->script_model.send_to_linked(
                        cog::message_type::exited,
                        /* sender */ attached_thing,
                        /* source */ tid,
                        /* source type */ presenter.model->get_thing_source_type(tid));
            });
    }
    else if(thing.attach_flags & flags::attach_flag::AttachedToWorldSurface) {
        maybe_if(thing.attached_surface, [&](surface_id attached_surface) {
                presenter.model->script_model.send_to_linked(
                        cog::message_type::exited,
                        /* sender */ attached_surface,
                        /* source */ tid,
                        /* source type */ presenter.model->get_thing_source_type(tid));
            });
    }

    thing.attach_flags = flag_set<flags::attach_flag>();

    cs.bus.fire_event(events::standing_material_changed(tid, flags::standing_material_type::none));
}

bool character_controller_aspect::step_on_surface(thing_id tid, components::thing& thing, surface_id surf_id,
                const physics::contact&) {
    const auto& surface = at_id(presenter.model->surfaces, surf_id);
    if(surface.flags & flags::surface_flag::Floor) {
        thing.attach_flags = flag_set<flags::attach_flag> { flags::attach_flag::AttachedToWorldSurface };
        thing.attached_surface = surf_id;

        presenter.model->script_model.send_to_linked(
                cog::message_type::entered,
                /* sender */ surface_id(surf_id),
                /* source */ thing_id(tid),
                /* source type */ presenter.model->get_thing_source_type(tid));

        cs.bus.fire_event(events::standing_material_changed(tid, get_standing_material(thing)));
        return true;
    }
    else {
        set_is_falling(tid, thing);
        return false;
    }

    // Player has landed.
    //presenter.AdjustThingPosition(thing_id, Math::VecBt(rrcb.m_hitPointWorld) + thing.Model3d->InsertOffset);
}

bool character_controller_aspect::step_on_thing(thing_id tid, components::thing& thing, thing_id land_thing_id,
                const physics::contact&) {
    const auto& attach_thing = presenter.model->get_thing(land_thing_id);
    if(attach_thing.flags & flags::thing_flag::CanStandOn) {
        thing.attach_flags = flag_set<flags::attach_flag> { flags::attach_flag::AttachedToThingFace };
        thing.attached_thing = land_thing_id;
        thing.prev_attached_thing_position = presenter.model->get_thing(land_thing_id).position;

        presenter.model->script_model.send_to_linked(
                cog::message_type::entered,
                /* sender */ thing_id(land_thing_id),
                /* source */ thing_id(tid),
                /* source type */ presenter.model->get_thing_source_type(tid));

        cs.bus.fire_event(events::standing_material_changed(tid, get_standing_material(thing)));
        return true;
    }
    else {
        set_is_falling(tid, thing);
        return false;
    }

    // Player has landed.
    //presenter.AdjustThingPosition(thing_id, Math::VecBt(rrcb.m_hitPointWorld) + thing.Model3d->InsertOffset);
}

void character_controller_aspect::land_on_surface(thing_id tid,
                                                  components::thing &thing,
                                                  surface_id surf_id,
                                                  physics::contact const &rrcb) {
    if(!step_on_surface(tid, thing, surf_id, rrcb)) {
        return;
    }

    cs.bus.fire_event(events::landed(tid));
}

void character_controller_aspect::land_on_thing(thing_id tid, components::thing& thing, thing_id land_thing_id,
                const physics::contact& rrcb) {
    if(!step_on_thing(tid, thing, land_thing_id, rrcb)) {
        return;
    }

    cs.bus.fire_event(events::landed(tid));
}

void character_controller_aspect::jump(thing_id tid, components::thing& thing) {
    cs.bus.fire_event(events::jumped(tid));
    set_is_falling(tid, thing);
    thing.vel = thing.vel + make_vector(0.0f, 0.0f, get<2>(thing.thrust));
}

void character_controller_aspect::update(gorc::time t,
                                         entity_id id,
                                         components::character&,
                                         components::thing &thing) {
    // Update actor state
    if(static_cast<int>(thing.attach_flags)) {
        update_standing(id, thing, t.elapsed_as_seconds());
    }
    else {
        update_falling(id, thing, t.elapsed_as_seconds());
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

void character_controller_aspect::on_killed(thing_id tid,
                                            components::thing &thing,
                                            thing_id killer) {
    presenter.model->script_model.send_to_linked(
            cog::message_type::killed,
            /* sender */ tid,
            /* source */ killer,
            /* source type */ presenter.model->get_thing_source_type(killer));

    thing.type = flags::thing_type::Corpse;
}

character_controller_aspect::character_controller_aspect(component_system &cs,
                                                         level_presenter &presenter)
    : inner_join_aspect(cs), presenter(presenter) {

    created_delegate =
        cs.bus.add_handler<events::thing_created>([&](events::thing_created const &e) {
        if(e.tpl.type == flags::thing_type::Actor ||
           e.tpl.type == flags::thing_type::Player ) {
            cs.emplace_component<components::character>(entity_id(e.thing));
        }

        if(e.tpl.type == flags::thing_type::Player) {
            // TODO: When player gets an aspect, this should be moved there.
            cs.emplace_component<components::player>(entity_id(e.thing));
        }
    });

    killed_delegate =
        cs.bus.add_handler<events::killed>([&](events::killed const &e) {
        auto rng = cs.find_component<components::character>(entity_id(e.thing));
        for(auto it = rng.begin(); it != rng.end(); ++it) {
            for(auto &thing : cs.find_component<components::thing>(entity_id(e.thing))) {
                on_killed(e.thing, thing.second, e.killer);
            }
        }
    });

    return;
}
