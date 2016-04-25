#include "log/log.hpp"
#include "level_presenter.hpp"
#include "level_model.hpp"
#include "game/constants.hpp"
#include "physics/query.hpp"
#include "libold/base/events/print.hpp"
#include "libold/base/events/exit.hpp"
#include "game/level_state.hpp"
#include "game/world/physics/physics_presenter.hpp"
#include "game/world/animations/animation_presenter.hpp"
#include "game/world/sounds/sound_presenter.hpp"
#include "game/world/keys/key_presenter.hpp"
#include "game/world/camera/camera_presenter.hpp"
#include "game/world/inventory/inventory_presenter.hpp"

#include "game/world/components/actor.hpp"
#include "game/world/components/character.hpp"
#include "game/world/components/player.hpp"
#include "game/world/components/item.hpp"
#include "game/world/components/weapon.hpp"
#include "game/world/components/class_sounds.hpp"
#include "game/world/components/puppet_animations.hpp"

#include "game/world/aspects/thing_controller_aspect.hpp"
#include "game/world/aspects/actor_controller_aspect.hpp"
#include "game/world/aspects/character_controller_aspect.hpp"
#include "game/world/aspects/item_controller_aspect.hpp"
#include "game/world/aspects/weapon_controller_aspect.hpp"
#include "game/world/aspects/dispatch_class_sound_aspect.hpp"
#include "game/world/aspects/puppet_animation_aspect.hpp"

#include "game/world/events/taken.hpp"
#include "game/world/events/killed.hpp"
#include "game/world/events/thing_created.hpp"
#include "game/world/events/armed_mode_changed.hpp"
#include "game/world/events/class_sound.hpp"

#include "jk/content/material.hpp"

gorc::game::world::level_presenter::level_presenter(level_state& components, const level_place& place)
    : components(components), place(place), contentmanager(place.contentmanager) {
    physics_presenter = std::make_unique<physics::physics_presenter>(*this);
    animation_presenter = std::make_unique<animations::animation_presenter>();
    sound_presenter = std::make_unique<sounds::sound_presenter>(*place.contentmanager);
    key_presenter = std::make_unique<keys::key_presenter>(*place.contentmanager);
    inventory_presenter = std::make_unique<inventory::inventory_presenter>(*this);
    camera_presenter = std::make_unique<camera::camera_presenter>(*this);

    return;
}

gorc::game::world::level_presenter::~level_presenter() {
    return;
}

void gorc::game::world::level_presenter::start(event_bus& eventBus) {
    eventbus = &eventBus;
    model = std::make_unique<level_model>(eventBus, *place.contentmanager, components.services, place.level,
            place.contentmanager->load<content::assets::inventory>("items.dat"));

    // Create local aspects
    model->ecs.emplace_aspect<aspects::thing_controller_aspect>(*this);
    model->ecs.emplace_aspect<aspects::actor_controller_aspect>();
    model->ecs.emplace_aspect<aspects::character_controller_aspect>(*this);
    model->ecs.emplace_aspect<aspects::item_controller_aspect>(*this);
    model->ecs.emplace_aspect<aspects::weapon_controller_aspect>(*this);
    model->ecs.emplace_aspect<aspects::dispatch_class_sound_aspect>(*this);
    model->ecs.emplace_aspect<aspects::puppet_animation_aspect>(*this);

    physics_presenter->start(*model, eventBus);
    key_presenter->start(*model, model->key_model, eventBus);
    camera_presenter->start(*model, model->camera_model);
    animation_presenter->start(*model);
    sound_presenter->start(*model, model->sound_model);
    inventory_presenter->start(*model, model->inventory_model);

    initialize_world();

    // Update all components
    update(gorc::time(timestamp(0), timestamp(0)));

    // Send startup and loading messages
    model->script_model.send_to_all(cog::message_type::startup,
                                    /* sender */ cog::value(),
                                    /* sender id */ cog::value(),
                                    /* source */ cog::value());

    model->script_model.send_to_all(cog::message_type::loading,
                                    /* sender */ cog::value(),
                                    /* sender id */ cog::value(),
                                    /* source */ cog::value());
    return;
}

void gorc::game::world::level_presenter::initialize_world() {
    // Create COG script instances.
    LOG_DEBUG("creating level cog instances");
    for(auto const &cog : model->level->cogs) {
        maybe_if_else(std::get<0>(cog), [&](auto script) {
                model->script_model.create_instance(script, std::get<1>(cog));
            },
            [&] {
                model->script_model.create_instance(contentmanager->load<cog::script>("dflt.cog"));
            });
    }

    // Create bin script instances.
    LOG_DEBUG("creating inventory cog instances");
    for(const auto& bin_tuple : *model->inventory_model.base_inventory) {
        const auto& bin = std::get<1>(bin_tuple);

        maybe_if(bin.cog, [&](auto cog) {
                model->script_model.create_global_instance(cog);
            });
    }

    // HACK: create thing collision shapes and rigid bodies, enumerate spawn points
    for(const auto& thing : model->level->things) {
        if(thing.type == flags::thing_type::Player) {
            // Add player spawn point and create ghost thing to fill ID.
            model->spawn_points.push_back(&thing);
            create_thing("none", thing.sector, thing.position, thing.orient);
        }
        else {
            create_thing(thing, thing.sector, thing.position, thing.orient);
        }
    }

    // HACK: Spawn camera thing
    model->current_spawn_point = 0;
    model->local_player_thing_id = create_thing(*model->spawn_points[model->current_spawn_point], model->spawn_points[model->current_spawn_point]->sector,
            model->spawn_points[model->current_spawn_point]->position, model->spawn_points[model->current_spawn_point]->orient);

    // Set camera focus to current local player thing.
    camera_presenter->set_camera_focus(0, model->local_player_thing_id);
    camera_presenter->set_camera_focus(1, model->local_player_thing_id);

    // Set coglinked flag on bound objects
    for(auto const &linkage : model->script_model.get_linkages()) {
        auto senderref = linkage.first;
        sector_id sec_id = senderref;
        surface_id surf_id = senderref;
        thing_id tid = senderref;

        switch(senderref.get_type()) {
        case cog::value_type::thing:
            if(tid.is_valid()) {
                model->get_thing(tid).flags += flags::thing_flag::CogLinked;
            }
            break;

        case cog::value_type::sector:
            if(sec_id.is_valid()) {
                at_id(model->sectors, sec_id).flags += flags::sector_flag::CogLinked;
            }
            break;

        case cog::value_type::surface:
            if(surf_id.is_valid()) {
                at_id(model->surfaces, surf_id).flags += flags::surface_flag::CogLinked;
            }
            break;

        default:
            continue;
        }
    }
}

void gorc::game::world::level_presenter::update(const gorc::time& time) {
    double dt = time.elapsed_as_seconds();

    physics_presenter->update(time);
    camera_presenter->update(time);
    sound_presenter->update(time);
    key_presenter->update(time);
    inventory_presenter->update(time);

    model->script_model.update(time_delta(time.elapsed_as_seconds()));
    model->ecs.update(time_delta(time.elapsed_as_seconds()));

    // update dynamic tint, game time.
    model->game_time += dt;
    model->level_time += dt;

    model->dynamic_tint = model->dynamic_tint * static_cast<float>(1.0 - dt);
    get<0>(model->dynamic_tint) = std::max(get<0>(model->dynamic_tint), 0.0f);
    get<1>(model->dynamic_tint) = std::max(get<1>(model->dynamic_tint), 0.0f);
    get<2>(model->dynamic_tint) = std::max(get<2>(model->dynamic_tint), 0.0f);

    for(auto thing_id : things_to_destroy) {
        real_destroy_thing(thing_id);
    }

    things_to_destroy.clear();
}

void gorc::game::world::level_presenter::update_thing_sector(thing_id tid, components::thing& thing,
        const vector<3>& oldThingPosition) {
    physics::segment segment(oldThingPosition, thing.position);
    physics::segment_adjoin_path(segment, *model, at_id(model->sectors, thing.sector), update_path_sector_scratch);

    if(std::get<0>(update_path_sector_scratch.back()) == thing.sector) {
        // thing hasn't moved to a different sector.
        return;
    }

    // Fire messages along path.
    surface_id first_adjoin = std::get<1>(update_path_sector_scratch.front());
    if(at_id(model->surfaces, first_adjoin).flags & flags::surface_flag::CogLinked) {
        model->send_to_linked(cog::message_type::crossed,
                              /* sender */ first_adjoin,
                              /* source */ tid);
    }

    for(unsigned int i = 1; i < update_path_sector_scratch.size() - 1; ++i) {
        if(at_id(model->sectors, thing.sector).flags & flags::sector_flag::CogLinked) {
            model->send_to_linked(cog::message_type::exited,
                                  /* sender */ thing.sector,
                                  /* source */ tid);
        }

        sector_id sec_id = std::get<0>(update_path_sector_scratch[i]);
        thing.sector = sec_id;
        if(at_id(model->sectors, sec_id).flags & flags::sector_flag::CogLinked) {
            model->send_to_linked(cog::message_type::entered,
                                  /* sender */ sec_id,
                                  /* source */ tid);
        }

        surface_id surf_id = std::get<1>(update_path_sector_scratch[i]);
        if(at_id(model->surfaces, surf_id).flags & flags::surface_flag::CogLinked) {
            model->send_to_linked(cog::message_type::crossed,
                                  /* sender */ surf_id,
                                  /* source */ tid);
        }
    }

    if(at_id(model->sectors, thing.sector).flags & flags::sector_flag::CogLinked) {
        model->send_to_linked(cog::message_type::exited,
                              /* sender */ thing.sector,
                              /* source */ tid);
    }

    sector_id last_sector = std::get<0>(update_path_sector_scratch.back());
    thing.sector = last_sector;
    if(at_id(model->sectors, last_sector).flags & flags::sector_flag::CogLinked) {
        model->send_to_linked(cog::message_type::entered,
                              /* sender */ last_sector,
                              /* source */ tid);
    }
}

void gorc::game::world::level_presenter::translate_camera(const vector<3>& amt) {
    auto& player = model->get_thing(model->local_player_thing_id);
    player.thrust = player.orient.transform(amt);

    // TODO: Handle this in character controller or in physics presenter.
    get<2>(player.thrust) = 0.0f;
}

void gorc::game::world::level_presenter::yaw_camera(double amt) {
    auto& player = model->get_thing(model->local_player_thing_id);
    get<1>(player.ang_vel) = static_cast<float>(amt) * 60.0f;
    //get<1>(player.orient) += amt;
}

void gorc::game::world::level_presenter::pitch_camera(double amt) {
    auto& player = model->get_thing(model->local_player_thing_id);
    player.head_pitch = clamp(player.head_pitch + static_cast<float>(amt), player.min_head_pitch, player.max_head_pitch);
}

void gorc::game::world::level_presenter::respawn() {
    ++model->current_spawn_point;
    model->current_spawn_point = model->current_spawn_point % model->spawn_points.size();

    components::thing& cameraThing = model->get_thing(model->local_player_thing_id);
    cameraThing.sector = model->spawn_points[model->current_spawn_point]->sector;
    cameraThing.position = model->spawn_points[model->current_spawn_point]->position;
    cameraThing.attach_flags = flag_set<flags::attach_flag>();
    cameraThing.vel = make_zero_vector<3, float>();
}

void gorc::game::world::level_presenter::jump() {
    auto& player = model->get_thing(model->local_player_thing_id);
    get<2>(player.thrust) = player.jump_speed;
}

void gorc::game::world::level_presenter::fly() {
    auto& player = model->get_thing(model->local_player_thing_id);
    get<2>(player.vel) += 0.1f;
}

void gorc::game::world::level_presenter::activate() {
    const auto& cam = model->camera_model.current_computed_state;
    auto activate_contact = physics_presenter->segment_query(physics::segment(cam.position, cam.position + cam.look * 0.5f),
            cam.containing_sector, get_local_player_thing(),
            [&](thing_id) { return true; },
            [&](surface_id s) {
                auto& surf = at_id(model->surfaces, s);
                return surf.flags & flags::surface_flag::CogLinked;
            });

    maybe_if(activate_contact, [&](physics::contact const &contact) {
        maybe_if(contact.contact_surface_id, [&](surface_id contact_surface_id) {
            LOG_INFO(format("ACTIVATE SURFACE: %d") % static_cast<int>(contact_surface_id));
            model->send_to_linked(cog::message_type::activated,
                                  /* sender */ contact_surface_id,
                                  /* source */ model->local_player_thing_id);
        });

        maybe_if(contact.contact_thing_id, [&](thing_id contact_thing_id) {
            LOG_INFO(format("ACTIVATE THING: %d") % static_cast<int>(contact_thing_id));
            model->send_to_linked(cog::message_type::activated,
                                  /* sender */ contact_thing_id,
                                  /* source */ model->local_player_thing_id);
            eventbus->fire_event(events::class_sound(contact_thing_id,
                                                     flags::sound_subclass_type::Activate));
        });
    });
}

void gorc::game::world::level_presenter::damage() {
    const auto& cam = model->camera_model.current_computed_state;
    auto activate_contact = physics_presenter->segment_query(physics::segment(cam.position, cam.position + cam.look * 0.5f),
            cam.containing_sector, get_local_player_thing(),
            [&](thing_id) { return true; },
            [&](surface_id s) {
                auto& surf = at_id(model->surfaces, s);
                return surf.flags & flags::surface_flag::CogLinked;
            });

    maybe_if(activate_contact, [&](physics::contact const &contact) {
        maybe_if(contact.contact_surface_id, [&](surface_id contact_surface_id) {
            model->send_to_linked(cog::message_type::damaged,
                                  /* sender */ contact_surface_id,
                                  /* source */ model->local_player_thing_id,
                                  /* param0 */ 1000,
                                  /* param1 */ static_cast<int>(flags::damage_flag::saber));
        });

        maybe_if(contact.contact_thing_id, [&](thing_id contact_thing_id) {
            damage_thing(contact_thing_id, 1000.0f, flag_set<flags::damage_flag> { flags::damage_flag::saber, flags::damage_flag::explosion},
                    model->local_player_thing_id);
        });
    });
}

void gorc::game::world::level_presenter::crouch(bool is_crouched) {
    auto& player = model->get_thing(model->local_player_thing_id);
    if(is_crouched) {
        player.physics_flags += flags::physics_flag::is_crouching;
    }
    else {
        player.physics_flags -= flags::physics_flag::is_crouching;
    }
}

void gorc::game::world::level_presenter::thing_sighted(thing_id tid) {
    model->get_thing(tid).flags += flags::thing_flag::Sighted;
    model->send_to_linked(cog::message_type::sighted,
                          /* sender */ tid,
                          /* source */ cog::value());
}

void gorc::game::world::level_presenter::ai_clear_mode(thing_id tid, flag_set<flags::ai_mode_flag> flags) {
    model->get_thing(tid).ai_mode_flags -= flags;
}

gorc::flag_set<gorc::flags::ai_mode_flag> gorc::game::world::level_presenter::ai_get_mode(thing_id tid) {
    return model->get_thing(tid).ai_mode_flags;
}

void gorc::game::world::level_presenter::ai_set_look_frame(thing_id tid, int frame) {
    auto& thing = model->get_thing(tid);

    if(frame >= 0 && frame < static_cast<int>(thing.frames.size())) {
        ai_set_look_pos(tid, std::get<0>(thing.frames[frame]));
    }
}

void gorc::game::world::level_presenter::ai_set_look_pos(thing_id tid, const vector<3>& pos) {
    auto& thing = model->get_thing(tid);

    thing.ai_look_target = pos - thing.position;
    thing.ai_mode_flags += flags::ai_mode_flag::turning_to_face_target;
}

void gorc::game::world::level_presenter::ai_set_mode(thing_id tid, flag_set<flags::ai_mode_flag> flags) {
    model->get_thing(tid).ai_mode_flags += flags;
}

void gorc::game::world::level_presenter::ai_set_move_frame(thing_id tid, int frame) {
    auto& thing = model->get_thing(tid);

    if(frame >= 0 && frame < static_cast<int>(thing.frames.size())) {
        ai_set_move_pos(tid, std::get<0>(thing.frames[frame]));
    }
}

void gorc::game::world::level_presenter::ai_set_move_pos(thing_id tid, const vector<3>& pos) {
    auto& thing = model->get_thing(tid);

    thing.ai_move_target = pos;
    thing.ai_mode_flags += flags::ai_mode_flag::moving_toward_destination;
}

void gorc::game::world::level_presenter::ai_set_move_speed(thing_id tid, float speed) {
    model->get_thing(tid).ai_move_speed = speed;
}

void gorc::game::world::level_presenter::ai_set_move_thing(thing_id tid, thing_id move_to_thing) {
    // TODO: Revisit. Does this mean continuously chase?
    ai_set_move_pos(tid, model->get_thing(move_to_thing).position);
}

// Color verbs
void gorc::game::world::level_presenter::add_dynamic_tint(thing_id, const color_rgb& tint) {
    model->dynamic_tint += tint;

    // Clamp dynamic tint
    auto dynamic_tint = model->dynamic_tint;
    get<0>(model->dynamic_tint) = std::max(get<0>(dynamic_tint), 0.0f);
    get<1>(model->dynamic_tint) = std::max(get<1>(dynamic_tint), 0.0f);
    get<2>(model->dynamic_tint) = std::max(get<2>(dynamic_tint), 0.0f);
    get<0>(model->dynamic_tint) = std::min(get<0>(dynamic_tint), 1.0f);
    get<1>(model->dynamic_tint) = std::min(get<1>(dynamic_tint), 1.0f);
    get<2>(model->dynamic_tint) = std::min(get<2>(dynamic_tint), 1.0f);
}

// Creature verbs
float gorc::game::world::level_presenter::get_thing_health(thing_id tid) {
    return model->get_thing(tid).health;
}

bool gorc::game::world::level_presenter::has_los(thing_id look_thing_id, thing_id target_thing_id) {
    auto& look_thing = model->get_thing(look_thing_id);
    auto& target_thing = model->get_thing(target_thing_id);

    auto contact = physics_presenter->thing_segment_query(look_thing_id, target_thing.position - look_thing.position,
            [&](thing_id tid) {
                if(tid == target_thing_id) {
                    return true;
                }

                auto& seen_thing = model->get_thing(tid);
                return seen_thing.type == flags::thing_type::cog;
            },
            [&](surface_id sid) {
                auto& surf = at_id(model->surfaces, sid);
                return surf.geometry_mode == flags::geometry_mode::solid && !(surf.face_type_flags & flags::face_flag::Translucent);
            });

    bool rv = false;
    maybe_if(contact, [&](physics::contact const &ct) {
        maybe_if(ct.contact_thing_id, [&](thing_id ctid) {
            rv = ctid == target_thing_id;
        });
    });

    return rv;
}

// Frame verbs
int gorc::game::world::level_presenter::get_cur_frame(thing_id tid) {
    return model->get_thing(tid).current_frame;
}

void gorc::game::world::level_presenter::jump_to_frame(thing_id tid, int frame, sector_id sector) {
    components::thing& referenced_thing = model->get_thing(tid);
    auto& referenced_frame = referenced_thing.frames[frame];
    set_thing_pos(tid, std::get<0>(referenced_frame), make_euler(std::get<1>(referenced_frame)), sector);
}

void gorc::game::world::level_presenter::move_to_frame(thing_id tid, int frame, float speed) {
    for(auto &thing : model->ecs.find_component<components::thing>(thing_id(tid))) {
        auto &referenced_thing = thing.second;
        referenced_thing.goal_frame = frame;
        if(frame > referenced_thing.current_frame) {
            referenced_thing.next_frame = referenced_thing.current_frame + 1;
        }
        else if(frame < referenced_thing.current_frame) {
            referenced_thing.next_frame = referenced_thing.current_frame - 1;
        }
        else {
            referenced_thing.next_frame = 1;
        }

        referenced_thing.path_move_speed = speed;
        referenced_thing.path_moving = true;
        referenced_thing.rotatepivot_moving = false;
        eventbus->fire_event(events::class_sound(tid, flags::sound_subclass_type::StartMove));
        sound_presenter->play_foley_loop_class(thing_id(tid), flags::sound_subclass_type::Moving);
    }
}

void gorc::game::world::level_presenter::path_move_pause(thing_id tid) {
    auto& referenced_thing = model->get_thing(tid);
    referenced_thing.path_moving_paused = true;
}

void gorc::game::world::level_presenter::path_move_resume(thing_id tid) {
    auto& referenced_thing = model->get_thing(tid);
    referenced_thing.path_moving_paused = false;
}

void gorc::game::world::level_presenter::rotate_pivot(thing_id tid, int frame, float time) {
    components::thing& referenced_thing = model->get_thing(tid);
    referenced_thing.path_moving = false;
    referenced_thing.rotatepivot_moving = true;
    referenced_thing.current_frame = 0;
    referenced_thing.goal_frame = frame;
    referenced_thing.path_move_speed = (fabs(time) <= 0.0) ? 1.0f : static_cast<float>(fabs(time));
    referenced_thing.path_move_time = 0.0f;
    referenced_thing.rotatepivot_longway = time < 0.0f;
    eventbus->fire_event(events::class_sound(tid, flags::sound_subclass_type::StartMove));
    sound_presenter->play_foley_loop_class(tid, flags::sound_subclass_type::Moving);
}

// level verbs
float gorc::game::world::level_presenter::get_game_time() {
    return static_cast<float>(model->game_time);
}

float gorc::game::world::level_presenter::get_level_time() {
    return static_cast<float>(model->level_time);
}

void gorc::game::world::level_presenter::jk_end_level(bool success) {
    if(success) {
        eventbus->fire_event(gorc::events::print("Ending level - success"));
    }
    else {
        eventbus->fire_event(gorc::events::print("Ending level - failure"));
    }

    eventbus->fire_event(gorc::events::exit());
}

// Misc verbs
void gorc::game::world::level_presenter::jk_disable_saber(thing_id tid) {
    auto& thing = model->get_thing(tid);
    thing.saber_enabled = false;
    return;
}

void gorc::game::world::level_presenter::jk_enable_saber(thing_id tid, float damage,
        float collide_length, float unknown) {
    auto& thing = model->get_thing(tid);
    thing.saber_enabled = true;
    thing.saber_damage = damage;
    thing.saber_collide_length = collide_length;
    thing.saber_unknown = unknown;
    return;
}

void gorc::game::world::level_presenter::jk_set_saber_info(thing_id tid,
        material_id side_mat, material_id tip_mat, float base_rad, float tip_rad, float length,
        thing_template_id wall, thing_template_id blood, thing_template_id saber) {
    auto& thing = model->get_thing(tid);

    thing.saber_side_mat = get_asset(*contentmanager, side_mat);
    thing.saber_tip_mat = get_asset(*contentmanager, tip_mat);
    thing.saber_base_rad = base_rad;
    thing.saber_tip_rad = tip_rad;
    thing.saber_length = length;

    thing.saber_wall = (wall.is_valid()) ? make_maybe(&at_id(model->level->templates, wall)) : nothing;
    thing.saber_blood = (blood.is_valid()) ? make_maybe(&at_id(model->level->templates, blood)) : nothing;
    thing.saber_saber = (saber.is_valid()) ? make_maybe(&at_id(model->level->templates, saber)) : nothing;
}

void gorc::game::world::level_presenter::take_item(thing_id taken_thing, thing_id player_id) {
    eventbus->fire_event(events::taken(taken_thing, player_id));
}

// Player verbs
gorc::thing_id gorc::game::world::level_presenter::get_local_player_thing() {
    return model->local_player_thing_id;
}

// sector verbs
void gorc::game::world::level_presenter::clear_sector_flags(sector_id sid, flag_set<flags::sector_flag> flags) {
    at_id(model->sectors, sid).flags -= flags;
}

gorc::thing_id gorc::game::world::level_presenter::first_thing_in_sector(sector_id sid) {
    for(auto& thing : model->ecs.all_components<components::thing>()) {
        if(thing.second.sector == sid) {
            return thing.first;
        }
    }

    return invalid_id;
}

gorc::flag_set<gorc::flags::sector_flag> gorc::game::world::level_presenter::get_sector_flags(sector_id sid) {
    return at_id(model->sectors, sid).flags;
}

gorc::thing_id gorc::game::world::level_presenter::next_thing_in_sector(thing_id tid) {
    sector_id sid = model->get_thing(tid).sector;

    for(auto& thing : model->ecs.all_components<components::thing>()) {
        if(thing.second.sector == sid && static_cast<int>(thing.first) > static_cast<int>(tid)) {
            return thing.first;
        }
    }

    return invalid_id;
}

void gorc::game::world::level_presenter::sector_sound(sector_id sid, sound_id sound, float volume) {
    auto& sector = at_id(model->sectors, sid);
    sector.ambient_sound = get_asset(*contentmanager, sound);
    sector.ambient_sound_volume = volume;
}

void gorc::game::world::level_presenter::set_sector_adjoins(sector_id sid, bool state) {
    content::assets::level_sector& sector = at_id(model->sectors, sid);
    for(int i = 0; i < sector.surface_count; ++i) {
        content::assets::level_surface& surface = model->surfaces[i + sector.first_surface];
        if(surface.adjoin >= 0) {
            content::assets::level_adjoin& adjoin = model->adjoins[surface.adjoin];

            if(state) {
                adjoin.flags += flags::adjoin_flag::Visible;
            }
            else {
                adjoin.flags -= flags::adjoin_flag::Visible;
            }
        }
    }
}

void gorc::game::world::level_presenter::set_sector_colormap(sector_id sid, colormap_id colormap) {
    at_id(model->sectors, sid).cmp = colormap.is_valid() ?
                                     make_maybe(get_asset(*place.contentmanager, colormap)) :
                                     nothing;
}

void gorc::game::world::level_presenter::set_sector_flags(sector_id sid, flag_set<flags::sector_flag> flags) {
    at_id(model->sectors, sid).flags += flags;
}

void gorc::game::world::level_presenter::set_sector_light(sector_id sid, float value, float) {
    // TODO: create animation to implement delay feature.
    content::assets::level_sector& sector = at_id(model->sectors, sid);
    sector.extra_light = value;
}

void gorc::game::world::level_presenter::set_sector_thrust(sector_id sid, const vector<3>& thrust) {
    content::assets::level_sector& sector = at_id(model->sectors, sid);
    sector.thrust = thrust * static_cast<float>(rate_factor);
}

void gorc::game::world::level_presenter::set_sector_tint(sector_id sid, const color_rgb& color) {
    at_id(model->sectors, sid).tint = color;
}

// surface verbs
void gorc::game::world::level_presenter::clear_adjoin_flags(surface_id surface, flag_set<flags::adjoin_flag> flags) {
    content::assets::level_surface& surf = at_id(model->surfaces, surface);
    if(surf.adjoin >= 0) {
        content::assets::level_adjoin& adj = model->adjoins[surf.adjoin];
        adj.flags -= flags;
    }
}

gorc::flags::geometry_mode gorc::game::world::level_presenter::get_face_geo_mode(surface_id surface) {
    return at_id(model->surfaces, surface).geometry_mode;
}

gorc::vector<3> gorc::game::world::level_presenter::get_surface_center(surface_id surface) {
    auto vec = make_zero_vector<3, float>();
    for(const auto& vx : at_id(model->level->surfaces, surface).vertices) {
        vec += model->level->vertices[std::get<0>(vx)];
    }

    vec = vec / static_cast<float>(at_id(model->level->surfaces, surface).vertices.size());
    return vec;
}

gorc::sector_id gorc::game::world::level_presenter::get_surface_sector(surface_id surface) {
    return at_id(model->surfaces, surface).get_object_data().sector;
}

void gorc::game::world::level_presenter::set_adjoin_flags(surface_id surface, flag_set<flags::adjoin_flag> flags) {
    content::assets::level_surface& surf = at_id(model->surfaces, surface);
    if(surf.adjoin >= 0) {
        content::assets::level_adjoin& adj = model->adjoins[surf.adjoin];
        adj.flags += flags;
    }
}

void gorc::game::world::level_presenter::set_face_geo_mode(surface_id surface, flags::geometry_mode geo_mode) {
    at_id(model->surfaces, surface).geometry_mode = geo_mode;
}

void gorc::game::world::level_presenter::set_face_type(surface_id surface, flag_set<flags::face_flag> face_flags) {
    at_id(model->surfaces, surface).face_type_flags += face_flags;
}

void gorc::game::world::level_presenter::set_surface_flags(surface_id surface, flag_set<flags::surface_flag> flags) {
    at_id(model->surfaces, surface).flags += flags;
}

// System verbs
gorc::sound_id gorc::game::world::level_presenter::load_sound(const char* fn) {
    return sound_id(static_cast<int>(place.contentmanager->load_id<content::assets::sound>(fn)));
}

// thing action verbs

gorc::thing_id gorc::game::world::level_presenter::create_thing(const content::assets::thing_template& tpl, sector_id sector_num,
        const vector<3>& pos, const quaternion<float>& orient) {
    // Initialize thing properties
    thing_id new_thing_id = model->ecs.make_entity();
    model->ecs.emplace_component<components::thing>(new_thing_id, tpl);

    auto& new_thing = model->get_thing(new_thing_id);

    new_thing.object_data.thing_id = static_cast<int>(new_thing_id);
    new_thing.sector = sector_num;
    new_thing.position = pos;
    new_thing.orient = orient;

    // Dispatch creation of thing components
    eventbus->fire_event(events::thing_created(new_thing_id, tpl));

    // Create thing components
    maybe_if(new_thing.cog, [&](auto cog) {
        model->script_model.create_global_instance(cog);
    });

    return new_thing_id;
}

gorc::thing_id gorc::game::world::level_presenter::create_thing(thing_template_id tpl_id, sector_id sector_num,
        const vector<3>& pos, const quaternion<float>& orientation) {
    return create_thing(at_id(model->level->templates, tpl_id), sector_num, pos, orientation);
}

gorc::thing_id gorc::game::world::level_presenter::create_thing(const std::string& tpl_name, sector_id sector_num,
        const vector<3>& pos, const quaternion<float>& orientation) {
    std::string temp;
    std::transform(tpl_name.begin(), tpl_name.end(), std::back_inserter(temp), tolower);
    auto it = model->level->template_map.find(temp);
    if(it != model->level->template_map.end()) {
        return create_thing(it->second, sector_num, pos, orientation);
    }
    else {
        // TODO: thing_template not found. report error.
        return invalid_id;
    }
}

gorc::thing_id gorc::game::world::level_presenter::fire_projectile(thing_id parent_thing_id, thing_template_id tpl_id, sound_id fire_sound_id, int puppet_submode_id,
        const vector<3>& offset_vec, const vector<3>& error_vec, float, int, float, float) {
    if(!parent_thing_id.is_valid() || !tpl_id.is_valid()) {
        return invalid_id;
    }

    const auto& parent_thing = model->get_thing(parent_thing_id);

    const auto& parent_look_orient = parent_thing.orient * make_rotation(make_vector(1.0f, 0.0f, 0.0f), parent_thing.head_pitch) * make_euler(error_vec);

    thing_id new_thing = create_thing(tpl_id, parent_thing.sector, parent_thing.position, parent_look_orient);
    auto& created_thing = model->get_thing(new_thing);

    created_thing.parent_thing = parent_thing_id;
    adjust_thing_pos(new_thing, created_thing.position + parent_look_orient.transform(offset_vec));

    // TODO: Don't orient velocity, let flags handle it.
    created_thing.vel = parent_look_orient.transform(created_thing.vel);

    // TODO: Deal with error vec, autoaim fov.

    if(fire_sound_id.is_valid()) {
        sound_presenter->play_sound_pos(fire_sound_id, created_thing.position, 1.0f, -1.0f, -1.0f, flag_set<flags::sound_flag>());
    }

    if(puppet_submode_id >= 0) {
        key_presenter->play_mode(thing_id(parent_thing_id), static_cast<flags::puppet_submode_type>(puppet_submode_id));
    }

    return new_thing;
}

void gorc::game::world::level_presenter::adjust_thing_pos(thing_id tid, const vector<3>& new_pos) {
    auto& thing = model->get_thing(tid);
    auto old_pos = thing.position;
    thing.position = new_pos;
    update_thing_sector(tid, thing, old_pos);
}

void gorc::game::world::level_presenter::set_thing_pos(thing_id tid, const vector<3>& new_pos, const quaternion<float>& new_orient, sector_id new_sector) {
    components::thing& thing = model->get_thing(tid);
    thing.position = new_pos;
    thing.orient = new_orient;
    thing.sector = new_sector;
}

void gorc::game::world::level_presenter::attach_thing_to_thing(thing_id tid, thing_id base_id) {
    auto& thing = model->get_thing(tid);
    auto& base = model->get_thing(base_id);

    thing.attach_flags = flag_set<flags::attach_flag> { flags::attach_flag::AttachedToThing };
    thing.attached_thing = base_id;
    thing.prev_attached_thing_position = base.position;
}

gorc::thing_id gorc::game::world::level_presenter::create_thing_at_thing(thing_template_id tpl_id, thing_id tid) {
    components::thing& referencedThing = model->get_thing(tid);
    thing_id new_thing_id = create_thing(tpl_id, referencedThing.sector, referencedThing.position, referencedThing.orient);
    components::thing& new_thing = model->get_thing(new_thing_id);

    new_thing.path_moving = false;

    maybe_if(new_thing.model_3d, [&](auto model) {
        this->adjust_thing_pos(new_thing_id, new_thing.position + model->insert_offset);
    });

    // CreateThingAtThing really does copy frames.
    std::transform(referencedThing.frames.begin(), referencedThing.frames.end(), std::back_inserter(new_thing.frames),
            [&new_thing](const std::tuple<vector<3>, vector<3>>& frame) -> std::tuple<vector<3>, vector<3>> {
        return maybe_if(new_thing.model_3d, frame, [&](auto model) {
            return std::make_tuple(std::get<0>(frame) + model->insert_offset, std::get<1>(frame));
        });
    });

    return new_thing_id;
}

float gorc::game::world::level_presenter::damage_thing(thing_id tid,
                                                       float damage,
                                                       flag_set<flags::damage_flag> flags,
                                                       thing_id damager_id) {
    model->send_to_linked(cog::message_type::damaged,
                          /* sender */ tid,
                          /* source */ damager_id,
                          /* param0 */ damage,
                          /* param1 */ static_cast<int>(flags));

    components::thing& referencedThing = model->get_thing(tid);
    if(referencedThing.health > 0.0f) {
        referencedThing.health -= damage;

        if(referencedThing.health <= 0.0f &&
           (referencedThing.type == flags::thing_type::Player ||
            referencedThing.type == flags::thing_type::Actor)) {
            eventbus->fire_event(events::killed(tid, damager_id));
        }
        else {
            eventbus->fire_event(events::class_sound(tid,
                                                     flags::sound_subclass_type::HurtSpecial));
            maybe_if(referencedThing.pup, [&](auto) {
                key_presenter->play_mode(thing_id(tid), flags::puppet_submode_type::Hit);
            });
        }
    }

    // TODO: Return actual 'undamaged' value as computed by thing cog with ReturnEx.

    return 0.0f;
}

void gorc::game::world::level_presenter::destroy_thing(thing_id tid) {
    things_to_destroy.insert(tid);
}

void gorc::game::world::level_presenter::real_destroy_thing(thing_id tid) {
    // Reset thing parentage.
    for(auto& thing_pair : model->ecs.all_components<components::thing>()) {
        if(thing_pair.second.parent_thing == tid) {
            thing_pair.second.parent_thing = invalid_id;
        }
    }

    // Expunge associated resources.
    key_presenter->expunge_thing_animations(thing_id(tid));

    model->ecs.erase_entity(tid);
}

void gorc::game::world::level_presenter::detach_thing(thing_id tid) {
    model->get_thing(tid).attach_flags = flag_set<flags::attach_flag>();
}

gorc::vector<3> gorc::game::world::level_presenter::get_thing_pos(thing_id tid) {
    components::thing& referenced_thing = model->get_thing(tid);
    return referenced_thing.position;
}

void gorc::game::world::level_presenter::heal_thing(thing_id tid, float amount) {
    components::thing& referencedThing = model->get_thing(tid);
    referencedThing.health = clamp(referencedThing.health + amount, 0.0f, referencedThing.max_health);
}

bool gorc::game::world::level_presenter::is_thing_moving(thing_id tid) {
    // TODO: Temporary hack implementation pending new physics implementation.
    components::thing& referencedThing = model->get_thing(tid);
    switch(referencedThing.move) {
    case flags::move_type::physics:
        return length(referencedThing.vel) > 0.0000001f;

    case flags::move_type::Path:
        return referencedThing.path_moving || referencedThing.rotatepivot_moving;

    default:
        return false;
    }
}

gorc::vector<3, float> gorc::game::world::level_presenter::get_thing_lvec(thing_id tid) {
    const auto& thing = model->get_thing(tid);
    return thing.orient.transform(make_vector(0.0f, 1.0f, 0.0f));
}

// thing flags verbs
void gorc::game::world::level_presenter::clear_actor_flags(thing_id tid, flag_set<flags::actor_flag> flags) {
    model->get_thing(tid).actor_flags -= flags;
}

gorc::flag_set<gorc::flags::actor_flag> gorc::game::world::level_presenter::get_actor_flags(thing_id tid) {
    return model->get_thing(tid).actor_flags;
}

void gorc::game::world::level_presenter::set_actor_flags(thing_id tid, flag_set<flags::actor_flag> flags) {
    model->get_thing(tid).actor_flags += flags;
}

void gorc::game::world::level_presenter::clear_thing_flags(thing_id tid, flag_set<flags::thing_flag> flags) {
    model->get_thing(tid).flags -= flags;
}

gorc::flag_set<gorc::flags::thing_flag> gorc::game::world::level_presenter::get_thing_flags(thing_id tid) {
    return model->get_thing(tid).flags;
}

void gorc::game::world::level_presenter::set_thing_flags(thing_id tid, flag_set<flags::thing_flag> flags) {
    model->get_thing(tid).flags += flags;
}

void gorc::game::world::level_presenter::clear_thing_attach_flags(thing_id tid, flag_set<flags::attach_flag> flags) {
    model->get_thing(tid).attach_flags -= flags;
}

gorc::flag_set<gorc::flags::attach_flag> gorc::game::world::level_presenter::get_thing_attach_flags(thing_id tid) {
    return model->get_thing(tid).attach_flags;
}

void gorc::game::world::level_presenter::set_thing_attach_flags(thing_id tid, flag_set<flags::attach_flag> flags) {
    model->get_thing(tid).attach_flags += flags;
}

void gorc::game::world::level_presenter::jk_clear_flags(thing_id tid, flag_set<flags::jk_flag> flags) {
    model->get_thing(tid).jk_flags -= flags;
}

gorc::flag_set<gorc::flags::jk_flag> gorc::game::world::level_presenter::jk_get_flags(thing_id tid) {
    return model->get_thing(tid).jk_flags;
}

void gorc::game::world::level_presenter::jk_set_flags(thing_id tid, flag_set<flags::jk_flag> flags) {
    model->get_thing(tid).jk_flags += flags;
}

// thing property verbs
gorc::thing_id gorc::game::world::level_presenter::get_thing_parent(thing_id tid) {
    return maybe_value(model->get_thing(tid).attached_thing, thing_id(invalid_id));
}

gorc::sector_id gorc::game::world::level_presenter::get_thing_sector(thing_id tid) {
    return model->get_thing(tid).sector;
}

gorc::flags::thing_type gorc::game::world::level_presenter::get_thing_type(thing_id tid) {
    return model->get_thing(tid).type;
}

void gorc::game::world::level_presenter::set_thing_light(thing_id tid, float light, float) {
    // TODO: Implement fade_time
    auto& thing = model->get_thing(tid);
    thing.light = light;
}

// velocity verbs

void gorc::game::world::level_presenter::stop_thing(thing_id thing) {
    auto& t = model->get_thing(thing);

    t.vel = make_zero_vector<3, float>();
    t.thrust = make_zero_vector<3, float>();
    t.ang_vel = make_zero_vector<3, float>();
    t.rot_thrust = make_zero_vector<3, float>();
}

// weapon verbs

void gorc::game::world::level_presenter::jk_set_weapon_mesh(thing_id player, model_id mesh) {
    model->get_thing(player).weapon_mesh = mesh.is_valid() ?
                                           make_maybe(get_asset(*contentmanager, mesh)) :
                                           nothing;
}

void gorc::game::world::level_presenter::set_armed_mode(thing_id player, flags::armed_mode mode) {
    eventbus->fire_event(events::armed_mode_changed(player, mode));
}

gorc::flags::puppet_mode_type gorc::game::world::level_presenter::get_major_mode(thing_id player) {
    for(auto const &pup : model->ecs.find_component<components::puppet_animations>(player)) {
        return pup.second.puppet_mode_type;
    }

    return flags::puppet_mode_type::unarmed;
}

void gorc::game::world::level_presenter::register_verbs(cog::verb_table &verbs, level_state &components) {

    camera::camera_presenter::register_verbs(verbs, components);
    animations::animation_presenter::register_verbs(verbs, components);
    sounds::sound_presenter::register_verbs(verbs, components);
    keys::key_presenter::register_verbs(verbs, components);
    inventory::inventory_presenter::register_verbs(verbs, components);
    physics::physics_presenter::register_verbs(verbs, components);

    // AI verbs
    verbs.add_safe_verb("aiclearmode", cog::value(), [&components](thing_id tid, int flags) {
        components.current_level_presenter->ai_clear_mode(tid, flag_set<flags::ai_mode_flag>(flags));
    });

    verbs.add_safe_verb("aigetmode", 0, [&components](thing_id tid) {
        return static_cast<int>(components.current_level_presenter->ai_get_mode(tid));
    });

    verbs.add_safe_verb("aisetlookframe", cog::value(), [&components](thing_id tid, int frame) {
        components.current_level_presenter->ai_set_look_frame(tid, frame);
    });

    verbs.add_safe_verb("aisetlookpos", cog::value(), [&components](thing_id tid, vector<3> pos) {
        components.current_level_presenter->ai_set_look_pos(tid, pos);
    });

    verbs.add_safe_verb("aisetmode", cog::value(), [&components](thing_id tid, int flags) {
        components.current_level_presenter->ai_set_mode(tid, flag_set<flags::ai_mode_flag>(flags));
    });

    verbs.add_safe_verb("aisetmoveframe", cog::value(), [&components](thing_id tid, int frame) {
        components.current_level_presenter->ai_set_move_frame(tid, frame);
    });

    verbs.add_safe_verb("aisetmovepos", cog::value(), [&components](thing_id tid, vector<3> pos) {
        components.current_level_presenter->ai_set_move_pos(tid, pos);
    });

    verbs.add_safe_verb("aisetmovespeed", cog::value(), [&components](thing_id tid, float speed) {
        components.current_level_presenter->ai_set_move_speed(tid, speed);
    });

    verbs.add_safe_verb("aisetmovething", cog::value(), [&components](thing_id tid, thing_id move_to_thing) {
        components.current_level_presenter->ai_set_move_thing(tid, move_to_thing);
    });

    // Color verbs
    verbs.add_safe_verb("adddynamictint", cog::value(), [&components](thing_id player_id, float r, float g, float b) {
        components.current_level_presenter->add_dynamic_tint(player_id, make_color(r, g, b));
    });

    // Creature verbs
    verbs.add_safe_verb("gethealth", 0.0f, [&components](thing_id tid) {
        return components.current_level_presenter->get_thing_health(tid);
    });

    verbs.add_safe_verb("getthinghealth", 0.0f, [&components](thing_id tid) {
        return components.current_level_presenter->get_thing_health(tid);
    });

    verbs.add_safe_verb("haslos", 0, [&components](thing_id look_thing, thing_id target) {
        return components.current_level_presenter->has_los(look_thing, target);
    });

    // Frame verbs
    verbs.add_safe_verb("getcurframe", 0, [&components](thing_id thing) {
        return components.current_level_presenter->get_cur_frame(thing);
    });

    verbs.add_safe_verb("jumptoframe", cog::value(), [&components](thing_id thing, int frame, sector_id sector) {
        return components.current_level_presenter->jump_to_frame(thing, frame, sector);
    });

    verbs.add_safe_verb("movetoframe", cog::value(), [&components](thing_id thing, int frame, float speed) {
        return components.current_level_presenter->move_to_frame(thing, frame, speed);
    });

    verbs.add_safe_verb("pathmovepause", cog::value(), [&components](thing_id thing) {
        components.current_level_presenter->path_move_pause(thing);
    });

    verbs.add_safe_verb("pathmoveresume", cog::value(), [&components](thing_id thing) {
        components.current_level_presenter->path_move_resume(thing);
    });

    verbs.add_safe_verb("rotatepivot", cog::value(), [&components](thing_id tid, int frame, float time) {
        components.current_level_presenter->rotate_pivot(tid, frame, time);
    });

    // level verbs
    verbs.add_verb("getgametime", [&components] {
        return components.current_level_presenter->get_game_time();
    });

    verbs.add_verb("getleveltime", [&components] {
        return components.current_level_presenter->get_level_time();
    });

    verbs.add_verb("jkendlevel", [&components] (int success) {
        components.current_level_presenter->jk_end_level(success);
    });

    // Misc verbs
    verbs.add_safe_verb("jkdisablesaber", cog::value(), [&components](thing_id tid) {
        components.current_level_presenter->jk_disable_saber(tid);
    });

    verbs.add_safe_verb("jkenablesaber", cog::value(), [&components](thing_id tid, float damage,
            float collide_length, float unknown) {
        components.current_level_presenter->jk_enable_saber(tid, damage, collide_length, unknown);
    });

    verbs.add_safe_verb("jksetsaberinfo", cog::value(), [&components](thing_id tid,
            material_id side_mat, material_id tip_mat, float base_rad, float tip_rad, float length,
            thing_template_id wall, thing_template_id blood, thing_template_id saber) {
        components.current_level_presenter->jk_set_saber_info(tid, side_mat, tip_mat,
                base_rad, tip_rad, length, wall, blood, saber);
    });

    verbs.add_safe_verb("takeitem", cog::value(), [&components](thing_id tid, thing_id player_id) {
        components.current_level_presenter->take_item(tid, player_id);
    });

    // Options verbs
    verbs.add_verb("getdifficulty", [] {
        // TODO: Add actual difficulty setting.
        return static_cast<int>(flags::difficulty_mode::medium);
    });

    verbs.add_verb("getautoswitch", [] {
        // TODO: Add actual autoswitch code.
        return 0x3;
    });

    verbs.add_verb("getautopickup", [] {
        // TODO: Add actual autopickup code.
        return 0xF;
    });

    verbs.add_verb("jkgetsabercam", [] {
        // TODO: Add actual sabercam code.
        return 1;
    });

    verbs.add_verb("ismulti", [] {
        // TODO: Return actual multiplayer state.
        return 0;
    });

    // Player verbs
    verbs.add_verb("getlocalplayerthing", [&components] {
        return components.current_level_presenter->get_local_player_thing();
    });

    verbs.add_verb("jkgetlocalplayer", [&components] {
        return components.current_level_presenter->get_local_player_thing();
    });

    // Print verbs:
    verbs.add_verb("jkprintunistring", [&components](int /*destination*/, int message_num) {
        // TODO: Add actual jkPrintUniString once localization is implemented.
        LOG_INFO(format("COG_%d") % message_num);
    });

    verbs.add_verb("print", [&components](const char* message) {
        // TODO: Add actual print.
        LOG_INFO(message);
    });

    verbs.add_verb("printint", [&components](int value) {
        // TODO: Add actual printint.
        LOG_INFO(format("%d") % value);
    });

    // sector verbs
    verbs.add_safe_verb("clearsectorflags", cog::value(), [&components](sector_id sid, int flags) {
        components.current_level_presenter->clear_sector_flags(sid, flag_set<flags::sector_flag>(flags));
    });

    verbs.add_safe_verb("getsectorflags", 0, [&components](sector_id sid) {
        return static_cast<int>(components.current_level_presenter->get_sector_flags(sid));
    });

    verbs.add_safe_verb("firstthinginsector", thing_id(invalid_id), [&components](sector_id sid) {
        return components.current_level_presenter->first_thing_in_sector(sid);
    });

    verbs.add_safe_verb("nextthinginsector", thing_id(invalid_id), [&components](thing_id tid) {
        return components.current_level_presenter->next_thing_in_sector(tid);
    });

    verbs.add_safe_verb("sectoradjoins", cog::value(), [&components](sector_id sid, int state) {
        components.current_level_presenter->set_sector_adjoins(sid, state);
    });

    verbs.add_safe_verb("sectorlight", cog::value(), [&components](sector_id sid, float light, float delay) {
        components.current_level_presenter->set_sector_light(sid, light, delay);
    });

    verbs.add_safe_verb("sectorsound", cog::value(), [&components](sector_id sid, sound_id sound, float volume) {
        components.current_level_presenter->sector_sound(sid, sound, volume);
    });

    verbs.add_safe_verb("sectorthrust", cog::value(), [&components](sector_id sid, vector<3> thrust_vec, float thrust_speed) {
        components.current_level_presenter->set_sector_thrust(sid, normalize(thrust_vec) * thrust_speed);
    });

    verbs.add_safe_verb("setcolormap", cog::value(), [&components](sector_id sid, colormap_id colormap) {
        components.current_level_presenter->set_sector_colormap(sid, colormap);
    });

    verbs.add_safe_verb("setsectoradjoins", cog::value(), [&components](sector_id sid, int state) {
        components.current_level_presenter->set_sector_adjoins(sid, state);
    });

    verbs.add_safe_verb("setsectorcolormap", cog::value(), [&components](sector_id sid, colormap_id colormap) {
        components.current_level_presenter->set_sector_colormap(sid, colormap);
    });

    verbs.add_safe_verb("setsectorflags", cog::value(), [&components](sector_id sid, int flags) {
        components.current_level_presenter->set_sector_flags(sid, flag_set<flags::sector_flag>(flags));
    });

    verbs.add_safe_verb("setsectorlight", cog::value(), [&components](sector_id sid, float light, float delay) {
        components.current_level_presenter->set_sector_light(sid, light, delay);
    });

    verbs.add_safe_verb("setsectorthrust", cog::value(), [&components](sector_id sid, vector<3> thrust_vec, float thrust_speed) {
        components.current_level_presenter->set_sector_thrust(sid, normalize(thrust_vec) * thrust_speed);
    });

    verbs.add_safe_verb("setsectortint", cog::value(), [&components](sector_id sid, color_rgb tint) {
        components.current_level_presenter->set_sector_tint(sid, tint);
    });

    // surface verbs
    verbs.add_safe_verb("clearadjoinflags", cog::value(), [&components](surface_id surface, int flags) {
        components.current_level_presenter->clear_adjoin_flags(surface, flag_set<flags::adjoin_flag>(flags));
    });

    verbs.add_safe_verb("getfacegeomode", cog::value(), [&components](surface_id surface) {
        return static_cast<int>(components.current_level_presenter->get_face_geo_mode(surface));
    });

    verbs.add_safe_verb("getsurfacecenter", make_zero_vector<3, float>(), [&components](surface_id surface) {
        return components.current_level_presenter->get_surface_center(surface);
    });

    verbs.add_safe_verb("getsurfacesector", sector_id(invalid_id), [&components](surface_id surface) {
        return components.current_level_presenter->get_surface_sector(surface);
    });

    verbs.add_safe_verb("setadjoinflags", cog::value(), [&components](surface_id surface, int flags) {
        components.current_level_presenter->set_adjoin_flags(surface, flag_set<flags::adjoin_flag>(flags));
    });

    verbs.add_safe_verb("setfacegeomode", cog::value(), [&components](surface_id surface, int mode) {
        components.current_level_presenter->set_face_geo_mode(surface, static_cast<flags::geometry_mode>(mode));
    });

    verbs.add_safe_verb("setfacetype", cog::value(), [&components](surface_id surface, int flags) {
        components.current_level_presenter->set_face_type(surface, flag_set<flags::face_flag>(flags));
    });

    verbs.add_safe_verb("setsurfaceflags", cog::value(), [&components](surface_id surface, int flags) {
        components.current_level_presenter->set_surface_flags(surface, flag_set<flags::surface_flag>(flags));
    });

    verbs.add_verb("setsurfacemat", [&components](int, int) {
        // TODO: Not implemented for now. Architecturally inconvenient.
    });

    verbs.add_safe_verb("surfacecenter", make_zero_vector<3, float>(), [&components](surface_id surface) {
        return components.current_level_presenter->get_surface_center(surface);
    });

    // System verbs
    verbs.add_verb("loadsound", [&components](const char* fn) {
        return components.current_level_presenter->load_sound(fn);
    });

    // thing action verbs
    verbs.add_safe_verb("attachthingtothing", cog::value(), [&components](thing_id attach_thing, thing_id base_thing) {
        components.current_level_presenter->attach_thing_to_thing(attach_thing, base_thing);
    });

    verbs.add_safe_verb("creatething", thing_id(invalid_id), [&components](thing_template_id tpl_id, thing_id thing_pos) {
        return components.current_level_presenter->create_thing_at_thing(tpl_id, thing_pos);
    });

    verbs.add_safe_verb("createthingatpos", thing_id(invalid_id), [&components](thing_template_id tpl_id, sector_id sid, vector<3> pos, vector<3> orient) {
        return components.current_level_presenter->create_thing(tpl_id, sid, pos, make_euler(orient));
    });

    verbs.add_verb("fireprojectile", [&components](thing_id parent_thing_id, thing_template_id tpl_id, sound_id snd_id, int submode_id,
            vector<3> offset_vec, vector<3> error_vec, float extra, int proj_flags, float aa_fovx, float aa_fovz) {
        return components.current_level_presenter->fire_projectile(parent_thing_id, tpl_id, snd_id, submode_id,
                offset_vec, error_vec, extra, proj_flags, aa_fovx, aa_fovz);
    });

    verbs.add_safe_verb("damagething", cog::value(), [&components](thing_id tid, float damage, int flags, thing_id damager_id) {
        return components.current_level_presenter->damage_thing(tid, damage, flag_set<flags::damage_flag>(flags), damager_id);
    });

    verbs.add_safe_verb("destroything", cog::value(), [&components](thing_id tid) {
        components.current_level_presenter->destroy_thing(tid);
    });

    verbs.add_safe_verb("detachthing", cog::value(), [&components](thing_id tid) {
        components.current_level_presenter->detach_thing(tid);
    });

    verbs.add_safe_verb("getthingpos", make_zero_vector<3, float>(), [&components](thing_id tid) {
        return components.current_level_presenter->get_thing_pos(tid);
    });

    verbs.add_safe_verb("healthing", cog::value(), [&components](thing_id tid, float amount) {
        components.current_level_presenter->heal_thing(tid, amount);
    });

    verbs.add_safe_verb("isthingmoving", false, [&components](thing_id tid) {
        return components.current_level_presenter->is_thing_moving(tid);
    });

    verbs.add_safe_verb("ismoving", false, [&components](thing_id tid) {
        return components.current_level_presenter->is_thing_moving(tid);
    });

    verbs.add_safe_verb("getthinglvec", make_zero_vector<3, float>(), [&components](thing_id tid) {
        return components.current_level_presenter->get_thing_lvec(tid);
    });

    // thing flags verbs
    verbs.add_safe_verb("clearactorflags", cog::value(), [&components](thing_id tid, int flags) {
        components.current_level_presenter->clear_actor_flags(tid, flag_set<flags::actor_flag>(flags));
    });

    verbs.add_safe_verb("getactorflags", 0, [&components](thing_id tid) {
        return static_cast<int>(components.current_level_presenter->get_actor_flags(tid));
    });

    verbs.add_safe_verb("setactorflags", cog::value(), [&components](thing_id tid, int flags) {
        components.current_level_presenter->set_actor_flags(tid, flag_set<flags::actor_flag>(flags));
    });

    verbs.add_safe_verb("clearthingflags", cog::value(), [&components](thing_id tid, int flags) {
        components.current_level_presenter->clear_thing_flags(tid, flag_set<flags::thing_flag>(flags));
    });

    verbs.add_safe_verb("getthingflags", 0, [&components](thing_id tid) {
        return static_cast<int>(components.current_level_presenter->get_thing_flags(tid));
    });

    verbs.add_safe_verb("setthingflags", cog::value(), [&components](thing_id tid, int flags) {
        components.current_level_presenter->set_thing_flags(tid, flag_set<flags::thing_flag>(flags));
    });

    verbs.add_safe_verb("clearthingattachflags", cog::value(), [&components](thing_id tid, int flags) {
        components.current_level_presenter->clear_thing_attach_flags(tid, flag_set<flags::attach_flag>(flags));
    });

    verbs.add_safe_verb("getthingattachflags", 0, [&components](thing_id tid) {
        return static_cast<int>(components.current_level_presenter->get_thing_attach_flags(tid));
    });

    verbs.add_safe_verb("setthingattachflags", cog::value(), [&components](thing_id tid, int flags) {
        components.current_level_presenter->set_thing_attach_flags(tid, flag_set<flags::attach_flag>(flags));
    });

    verbs.add_safe_verb("jkclearflags", cog::value(), [&components](thing_id tid, int flags) {
        components.current_level_presenter->jk_clear_flags(tid, flag_set<flags::jk_flag>(flags));
    });

    verbs.add_safe_verb("jkgetflags", 0, [&components](thing_id tid) {
        return static_cast<int>(components.current_level_presenter->jk_get_flags(tid));
    });

    verbs.add_safe_verb("jksetflags", cog::value(), [&components](thing_id tid, int flags) {
        components.current_level_presenter->jk_set_flags(tid, flag_set<flags::jk_flag>(flags));
    });

    // thing property verbs
    verbs.add_safe_verb("getthingparent", thing_id(invalid_id), [&components](thing_id tid) {
        return components.current_level_presenter->get_thing_parent(tid);
    });

    verbs.add_safe_verb("getthingsector", sector_id(invalid_id), [&components](thing_id tid) {
        return components.current_level_presenter->get_thing_sector(tid);
    });

    verbs.add_safe_verb("getthingtype", 0, [&components](thing_id tid) {
        return static_cast<int>(components.current_level_presenter->get_thing_type(tid));
    });

    verbs.add_safe_verb("setthinglight", cog::value(), [&components](thing_id tid, float light, float fade_time) {
        components.current_level_presenter->set_thing_light(tid, light, fade_time);
    });

    verbs.add_safe_verb("thinglight", cog::value(), [&components](thing_id tid, float light, float fade_time) {
        components.current_level_presenter->set_thing_light(tid, light, fade_time);
    });

    // velocity verbs
    verbs.add_safe_verb("stopthing", cog::value(), [&components](thing_id tid) {
        components.current_level_presenter->stop_thing(tid);
    });

    verbs.add_safe_verb("setthingrotvel", cog::value(), [&components](thing_id tid, vector<3> vel) {
        // TODO: Proper implementation
        components.current_level_presenter->model->get_thing(tid).ang_vel = vel;
    });

    verbs.add_safe_verb("applyforce", cog::value(), [&components](thing_id tid, vector<3> force) {
        // TODO: Proper implementation
        components.current_level_presenter->model->get_thing(tid).vel += force;
    });

    verbs.add_safe_verb("getthingthrust", 0.0f, [&components](thing_id tid) {
        // TODO: Proper implementation
        return components.current_level_presenter->model->get_thing(tid).thrust;
    });

    // weapon verbs
    verbs.add_safe_verb("jksetweaponmesh", cog::value(), [&components](thing_id tid, model_id mesh_id) {
        components.current_level_presenter->jk_set_weapon_mesh(tid, mesh_id);
    });

    verbs.add_safe_verb("setarmedmode", cog::value(), [&components](thing_id player, int mode) {
        components.current_level_presenter->set_armed_mode(player, flags::armed_mode(mode));
    });

    verbs.add_safe_verb("getmajormode", 0, [&components](thing_id player) {
        return static_cast<int>(components.current_level_presenter->get_major_mode(player));
    });
}
