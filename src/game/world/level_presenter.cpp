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
#include "game/world/scripts/script_presenter.hpp"
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

gorc::game::world::level_presenter::level_presenter(level_state& components, const level_place& place)
    : components(components), place(place), contentmanager(place.contentmanager) {
    physics_presenter = std::make_unique<physics::physics_presenter>(*this);
    animation_presenter = std::make_unique<animations::animation_presenter>();
    script_presenter = std::make_unique<scripts::script_presenter>(components);
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
    model = std::make_unique<level_model>(eventBus, *place.contentmanager, components.compiler, place.level,
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
    script_presenter->start(*model, model->script_model);
    sound_presenter->start(*model, model->sound_model);
    inventory_presenter->start(*model, model->inventory_model);

    initialize_world();

    // Update all components
    update(gorc::time(timestamp(0), timestamp(0)));

    // Send startup and loading messages
    script_presenter->send_message_to_all(cog::message_id::loading, -1, -1, flags::message_type::nothing);

    return;
}

void gorc::game::world::level_presenter::initialize_world() {
    script_presenter->create_level_dummy_instances(model->level->cogs.size());

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

    // create bin script instances.
    for(const auto& bin_tuple : *model->inventory_model.base_inventory) {
        const auto& bin = std::get<1>(bin_tuple);

        maybe_if(bin.cog, [&](auto cog) {
            script_presenter->create_global_cog_instance(cog->cogscript, *place.contentmanager, components.compiler);
        });
    }

    // create COG script instances.
    for(unsigned int i = 0; i < model->level->cogs.size(); ++i) {
        const auto& cog = model->level->cogs[i];
        maybe_if(std::get<0>(cog), [&](auto script) {
            const std::vector<cog::vm::value>& values = std::get<1>(cog);

            script_presenter->create_level_cog_instance(i, script->cogscript, *place.contentmanager, components.compiler, values);
        });
    }
}

void gorc::game::world::level_presenter::update(const gorc::time& time) {
    double dt = time.elapsed_as_seconds();

    physics_presenter->update(time);
    camera_presenter->update(time);
    script_presenter->update(time);
    sound_presenter->update(time);
    key_presenter->update(time);
    inventory_presenter->update(time);

    model->ecs.update(time);

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

void gorc::game::world::level_presenter::update_thing_sector(int thing_id, components::thing& thing,
        const vector<3>& oldThingPosition) {
    physics::segment segment(oldThingPosition, thing.position);
    physics::segment_adjoin_path(segment, *model, model->sectors[thing.sector], update_path_sector_scratch);

    if(std::get<0>(update_path_sector_scratch.back()) == thing.sector) {
        // thing hasn't moved to a different sector.
        return;
    }

    // Fire messages along path.
    unsigned int first_adjoin = std::get<1>(update_path_sector_scratch.front());
    if(model->surfaces[first_adjoin].flags & flags::surface_flag::CogLinked) {
        script_presenter->send_message_to_linked(cog::message_id::crossed,
                first_adjoin, flags::message_type::surface,
                thing_id, flags::message_type::thing);
    }

    for(unsigned int i = 1; i < update_path_sector_scratch.size() - 1; ++i) {
        if(model->sectors[thing.sector].flags & flags::sector_flag::CogLinked) {
            script_presenter->send_message_to_linked(cog::message_id::exited,
                    thing.sector, flags::message_type::sector,
                    thing_id, flags::message_type::thing);
        }

        unsigned int sec_id = std::get<0>(update_path_sector_scratch[i]);
        thing.sector = sec_id;
        if(model->sectors[sec_id].flags & flags::sector_flag::CogLinked) {
            script_presenter->send_message_to_linked(cog::message_id::entered,
                    sec_id, flags::message_type::sector,
                    static_cast<int>(thing_id), flags::message_type::thing);
        }

        unsigned int surf_id = std::get<1>(update_path_sector_scratch[i]);
        if(model->surfaces[surf_id].flags & flags::surface_flag::CogLinked) {
            script_presenter->send_message_to_linked(cog::message_id::crossed,
                    surf_id, flags::message_type::surface,
                    static_cast<int>(thing_id), flags::message_type::thing);
        }
    }

    if(model->sectors[thing.sector].flags & flags::sector_flag::CogLinked) {
        script_presenter->send_message_to_linked(cog::message_id::exited,
                thing.sector, flags::message_type::sector,
                thing_id, flags::message_type::thing);
    }

    unsigned int last_sector = std::get<0>(update_path_sector_scratch.back());
    thing.sector = last_sector;
    if(model->sectors[last_sector].flags & flags::sector_flag::CogLinked) {
        script_presenter->send_message_to_linked(cog::message_id::entered,
                last_sector, flags::message_type::sector,
                static_cast<int>(thing_id), flags::message_type::thing);
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
            [&](int) { return true; },
            [&](int s) {
                auto& surf = model->surfaces[s];
                return surf.flags & flags::surface_flag::CogLinked;
            });

    maybe_if(activate_contact, [&](physics::contact const &contact) {
        maybe_if(contact.contact_surface_id, [&](int contact_surface_id) {
            LOG_INFO(format("ACTIVATE SURFACE: %d") % contact_surface_id);
            script_presenter->send_message_to_linked(cog::message_id::activated,
                    contact_surface_id, flags::message_type::surface,
                    model->local_player_thing_id, flags::message_type::thing);
        });

        maybe_if(contact.contact_thing_id, [&](int contact_thing_id) {
            LOG_INFO(format("ACTIVATE THING: %d") % contact_thing_id);
            script_presenter->send_message_to_linked(cog::message_id::activated, contact_thing_id, flags::message_type::thing,
                    model->local_player_thing_id, flags::message_type::thing);
            eventbus->fire_event(events::class_sound(entity_id(contact_thing_id),
                                                     flags::sound_subclass_type::Activate));
        });
    });
}

void gorc::game::world::level_presenter::damage() {
    const auto& cam = model->camera_model.current_computed_state;
    auto activate_contact = physics_presenter->segment_query(physics::segment(cam.position, cam.position + cam.look * 0.5f),
            cam.containing_sector, get_local_player_thing(),
            [&](int) { return true; },
            [&](int s) {
                auto& surf = model->surfaces[s];
                return surf.flags & flags::surface_flag::CogLinked;
            });

    maybe_if(activate_contact, [&](physics::contact const &contact) {
        maybe_if(contact.contact_surface_id, [&](int contact_surface_id) {
            script_presenter->send_message_to_linked(cog::message_id::damaged, contact_surface_id, flags::message_type::surface,
                    model->local_player_thing_id, flags::message_type::thing, 1000, static_cast<int>(flags::damage_flag::saber));
        });

        maybe_if(contact.contact_thing_id, [&](int contact_thing_id) {
            damage_thing(entity_id(contact_thing_id), 1000.0f, flag_set<flags::damage_flag> { flags::damage_flag::saber, flags::damage_flag::explosion},
                    entity_id(model->local_player_thing_id));
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

void gorc::game::world::level_presenter::thing_sighted(int thing_id) {
    model->get_thing(thing_id).flags += flags::thing_flag::Sighted;
    script_presenter->send_message_to_linked(cog::message_id::sighted, thing_id, flags::message_type::thing);
}


void gorc::game::world::level_presenter::ai_clear_mode(int thing_id, flag_set<flags::ai_mode_flag> flags) {
    model->get_thing(thing_id).ai_mode_flags -= flags;
}

gorc::flag_set<gorc::flags::ai_mode_flag> gorc::game::world::level_presenter::ai_get_mode(int thing_id) {
    return model->get_thing(thing_id).ai_mode_flags;
}

void gorc::game::world::level_presenter::ai_set_look_frame(int thing_id, int frame) {
    auto& thing = model->get_thing(thing_id);

    if(frame >= 0 && frame < static_cast<int>(thing.frames.size())) {
        ai_set_look_pos(thing_id, std::get<0>(thing.frames[frame]));
    }
}

void gorc::game::world::level_presenter::ai_set_look_pos(int thing_id, const vector<3>& pos) {
    auto& thing = model->get_thing(thing_id);

    thing.ai_look_target = pos - thing.position;
    thing.ai_mode_flags += flags::ai_mode_flag::turning_to_face_target;
}

void gorc::game::world::level_presenter::ai_set_mode(int thing_id, flag_set<flags::ai_mode_flag> flags) {
    model->get_thing(thing_id).ai_mode_flags += flags;
}

void gorc::game::world::level_presenter::ai_set_move_frame(int thing_id, int frame) {
    auto& thing = model->get_thing(thing_id);

    if(frame >= 0 && frame < static_cast<int>(thing.frames.size())) {
        ai_set_move_pos(thing_id, std::get<0>(thing.frames[frame]));
    }
}

void gorc::game::world::level_presenter::ai_set_move_pos(int thing_id, const vector<3>& pos) {
    auto& thing = model->get_thing(thing_id);

    thing.ai_move_target = pos;
    thing.ai_mode_flags += flags::ai_mode_flag::moving_toward_destination;
}

void gorc::game::world::level_presenter::ai_set_move_speed(int thing_id, float speed) {
    model->get_thing(thing_id).ai_move_speed = speed;
}

void gorc::game::world::level_presenter::ai_set_move_thing(int thing_id, int move_to_thing) {
    // TODO: Revisit. Does this mean continuously chase?
    ai_set_move_pos(thing_id, model->get_thing(move_to_thing).position);
}

// Color verbs
void gorc::game::world::level_presenter::add_dynamic_tint(int, const vector<3>& tint) {
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
float gorc::game::world::level_presenter::get_thing_health(int thing_id) {
    return model->get_thing(thing_id).health;
}

bool gorc::game::world::level_presenter::has_los(int look_thing_id, int target_thing_id) {
    auto& look_thing = model->get_thing(look_thing_id);
    auto& target_thing = model->get_thing(target_thing_id);

    auto contact = physics_presenter->thing_segment_query(look_thing_id, target_thing.position - look_thing.position,
            [&](int tid) {
                if(tid == target_thing_id) {
                    return true;
                }

                auto& seen_thing = model->get_thing(tid);
                return seen_thing.type == flags::thing_type::cog;
            },
            [&](int sid) {
                auto& surf = model->surfaces[sid];
                return surf.geometry_mode == flags::geometry_mode::solid && !(surf.face_type_flags & flags::face_flag::Translucent);
            });

    bool rv = false;
    maybe_if(contact, [&](physics::contact const &ct) {
        maybe_if(ct.contact_thing_id, [&](int ctid) {
            rv = ctid == target_thing_id;
        });
    });

    return rv;
}

// Frame verbs
int gorc::game::world::level_presenter::get_cur_frame(entity_id thing_id) {
    return model->get_thing(thing_id).current_frame;
}

void gorc::game::world::level_presenter::jump_to_frame(entity_id thing_id, int frame, int sector) {
    components::thing& referenced_thing = model->get_thing(thing_id);
    auto& referenced_frame = referenced_thing.frames[frame];
    set_thing_pos(thing_id, std::get<0>(referenced_frame), make_euler(std::get<1>(referenced_frame)), sector);
}

void gorc::game::world::level_presenter::move_to_frame(entity_id thing_id, int frame, float speed) {
    for(auto &thing : model->ecs.find_component<components::thing>(thing_id)) {
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
        eventbus->fire_event(events::class_sound(thing_id, flags::sound_subclass_type::StartMove));
        sound_presenter->play_foley_loop_class(thing_id, flags::sound_subclass_type::Moving);
    }
}

void gorc::game::world::level_presenter::path_move_pause(entity_id thing_id) {
    auto& referenced_thing = model->get_thing(thing_id);
    referenced_thing.path_moving_paused = true;
}

void gorc::game::world::level_presenter::path_move_resume(entity_id thing_id) {
    auto& referenced_thing = model->get_thing(thing_id);
    referenced_thing.path_moving_paused = false;
}

void gorc::game::world::level_presenter::rotate_pivot(entity_id thing_id, int frame, float time) {
    components::thing& referenced_thing = model->get_thing(thing_id);
    referenced_thing.path_moving = false;
    referenced_thing.rotatepivot_moving = true;
    referenced_thing.current_frame = 0;
    referenced_thing.goal_frame = frame;
    referenced_thing.path_move_speed = (fabs(time) <= 0.0) ? 1.0f : static_cast<float>(fabs(time));
    referenced_thing.path_move_time = 0.0f;
    referenced_thing.rotatepivot_longway = time < 0.0f;
    eventbus->fire_event(events::class_sound(thing_id, flags::sound_subclass_type::StartMove));
    sound_presenter->play_foley_loop_class(thing_id, flags::sound_subclass_type::Moving);
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
void gorc::game::world::level_presenter::jk_disable_saber(int thing_id) {
    auto& thing = model->get_thing(thing_id);
    thing.saber_enabled = false;
    return;
}

void gorc::game::world::level_presenter::jk_enable_saber(int thing_id, float damage,
        float collide_length, float unknown) {
    auto& thing = model->get_thing(thing_id);
    thing.saber_enabled = true;
    thing.saber_damage = damage;
    thing.saber_collide_length = collide_length;
    thing.saber_unknown = unknown;
    return;
}

void gorc::game::world::level_presenter::jk_set_saber_info(int thing_id,
        int side_mat, int tip_mat, float base_rad, float tip_rad, float length,
        int wall, int blood, int saber) {
    auto& thing = model->get_thing(thing_id);

    thing.saber_side_mat = contentmanager->get_asset<content::assets::material>(side_mat);
    thing.saber_tip_mat = contentmanager->get_asset<content::assets::material>(tip_mat);
    thing.saber_base_rad = base_rad;
    thing.saber_tip_rad = tip_rad;
    thing.saber_length = length;

    thing.saber_wall = (wall >= 0) ? &model->level->templates[wall] : nullptr;
    thing.saber_blood = (blood >= 0) ? &model->level->templates[blood] : nullptr;
    thing.saber_saber = (saber >= 0) ? &model->level->templates[saber] : nullptr;
}

void gorc::game::world::level_presenter::take_item(entity_id thing_id, entity_id player_id) {
    eventbus->fire_event(events::taken(thing_id, player_id));
}

// Player verbs
int gorc::game::world::level_presenter::get_local_player_thing() {
    return model->local_player_thing_id;
}

// sector verbs
void gorc::game::world::level_presenter::clear_sector_flags(int sector_id, flag_set<flags::sector_flag> flags) {
    model->sectors[sector_id].flags -= flags;
}

int gorc::game::world::level_presenter::first_thing_in_sector(int sector_id) {
    for(auto& thing : model->ecs.all_components<components::thing>()) {
        if(thing.second.sector == sector_id) {
            return static_cast<int>(thing.first);
        }
    }

    return -1;
}

gorc::flag_set<gorc::flags::sector_flag> gorc::game::world::level_presenter::get_sector_flags(int sector_id) {
    return model->sectors[sector_id].flags;
}

int gorc::game::world::level_presenter::next_thing_in_sector(int thing_id) {
    int sector_id = model->get_thing(thing_id).sector;

    for(auto& thing : model->ecs.all_components<components::thing>()) {
        if(thing.second.sector == sector_id && static_cast<int>(thing.first) > thing_id) {
            return static_cast<int>(thing.first);
        }
    }

    return -1;
}

void gorc::game::world::level_presenter::sector_sound(int sector_id, int sound, float volume) {
    auto& sector = model->sectors[sector_id];
    sector.ambient_sound = contentmanager->get_asset<content::assets::sound>(sound);
    sector.ambient_sound_volume = volume;
}

void gorc::game::world::level_presenter::set_sector_adjoins(int sector_id, bool state) {
    content::assets::level_sector& sector = model->sectors[sector_id];
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

void gorc::game::world::level_presenter::set_sector_colormap(int sector_id, int colormap) {
    auto& sector = model->sectors[sector_id];
    sector.cmp = place.contentmanager->get_asset<content::assets::colormap>(colormap);
}

void gorc::game::world::level_presenter::set_sector_flags(int sector_id, flag_set<flags::sector_flag> flags) {
    model->sectors[sector_id].flags += flags;
}

void gorc::game::world::level_presenter::set_sector_light(int sector_id, float value, float) {
    // TODO: create animation to implement delay feature.
    content::assets::level_sector& sector = model->sectors[sector_id];
    sector.extra_light = value;
}

void gorc::game::world::level_presenter::set_sector_thrust(int sector_id, const vector<3>& thrust) {
    content::assets::level_sector& sector = model->sectors[sector_id];
    sector.thrust = thrust * static_cast<float>(rate_factor);
}

void gorc::game::world::level_presenter::set_sector_tint(int sector_id, const vector<3>& color) {
    content::assets::level_sector& sector = model->sectors[sector_id];
    sector.tint = color;
}

// surface verbs
void gorc::game::world::level_presenter::clear_adjoin_flags(int surface, flag_set<flags::adjoin_flag> flags) {
    content::assets::level_surface& surf = model->surfaces[surface];
    if(surf.adjoin >= 0) {
        content::assets::level_adjoin& adj = model->adjoins[surf.adjoin];
        adj.flags -= flags;
    }
}

gorc::flags::geometry_mode gorc::game::world::level_presenter::get_face_geo_mode(int surface) {
    return model->surfaces[surface].geometry_mode;
}

gorc::vector<3> gorc::game::world::level_presenter::get_surface_center(int surface) {
    auto vec = make_zero_vector<3, float>();
    for(const auto& vx : model->level->surfaces[surface].vertices) {
        vec += model->level->vertices[std::get<0>(vx)];
    }

    vec = vec / static_cast<float>(model->level->surfaces[surface].vertices.size());
    return vec;
}

int gorc::game::world::level_presenter::get_surface_sector(int surface) {
    for(const auto& sec : model->sectors) {
        if(surface >= sec.first_surface && surface < (sec.first_surface + sec.surface_count)) {
            return sec.number;
        }
    }

    return -1;
}

void gorc::game::world::level_presenter::set_adjoin_flags(int surface, flag_set<flags::adjoin_flag> flags) {
    content::assets::level_surface& surf = model->surfaces[surface];
    if(surf.adjoin >= 0) {
        content::assets::level_adjoin& adj = model->adjoins[surf.adjoin];
        adj.flags += flags;
    }
}

void gorc::game::world::level_presenter::set_face_geo_mode(int surface, flags::geometry_mode geo_mode) {
    if(surface >= 0) {
        model->surfaces[surface].geometry_mode = geo_mode;
    }
}

void gorc::game::world::level_presenter::set_face_type(int surface, flag_set<flags::face_flag> face_flags) {
    model->surfaces[surface].face_type_flags += face_flags;
}

void gorc::game::world::level_presenter::set_surface_flags(int surface, flag_set<flags::surface_flag> flags) {
    content::assets::level_surface& surf = model->surfaces[surface];
    surf.flags += flags;
}

// System verbs
int gorc::game::world::level_presenter::load_sound(const char* fn) {
    return place.contentmanager->load_id<content::assets::sound>(fn);
}

// thing action verbs

int gorc::game::world::level_presenter::create_thing(const content::assets::thing_template& tpl, unsigned int sector_num,
        const vector<3>& pos, const quaternion<float>& orient) {
    // Initialize thing properties
    entity_id new_thing_id = model->ecs.make_entity();
    model->ecs.emplace_component<components::thing>(new_thing_id, tpl);

    auto& new_thing = model->get_thing(static_cast<int>(new_thing_id));

    new_thing.object_data.thing_id = static_cast<int>(new_thing_id);
    new_thing.sector = sector_num;
    new_thing.position = pos;
    new_thing.orient = orient;

    // Dispatch creation of thing components
    eventbus->fire_event(events::thing_created(new_thing_id, tpl));

    // Create thing components
    maybe_if(new_thing.cog, [&](auto cog) {
        script_presenter->create_global_cog_instance(cog->cogscript, *place.contentmanager, components.compiler);
    });

    return static_cast<int>(new_thing_id);
}

int gorc::game::world::level_presenter::create_thing(int tpl_id, unsigned int sector_num,
        const vector<3>& pos, const quaternion<float>& orientation) {
    if(tpl_id < 0) {
        // TODO: thing_template not found. report error.
        return -1;
    }

    return create_thing(model->level->templates[tpl_id], sector_num, pos, orientation);
}

int gorc::game::world::level_presenter::create_thing(const std::string& tpl_name, unsigned int sector_num,
        const vector<3>& pos, const quaternion<float>& orientation) {
    std::string temp;
    std::transform(tpl_name.begin(), tpl_name.end(), std::back_inserter(temp), tolower);
    auto it = model->level->template_map.find(temp);
    if(it != model->level->template_map.end()) {
        return create_thing(it->second, sector_num, pos, orientation);
    }
    else {
        // TODO: thing_template not found. report error.
        return -1;
    }
}

int gorc::game::world::level_presenter::fire_projectile(entity_id parent_thing_id, int tpl_id, int fire_sound_id, int puppet_submode_id,
        const vector<3>& offset_vec, const vector<3>& error_vec, float, int, float, float) {
    const auto& parent_thing = model->get_thing(parent_thing_id);

    const auto& parent_look_orient = parent_thing.orient * make_rotation(make_vector(1.0f, 0.0f, 0.0f), parent_thing.head_pitch) * make_euler(error_vec);

    int new_thing = create_thing(tpl_id, parent_thing.sector, parent_thing.position, parent_look_orient);
    auto& created_thing = model->get_thing(new_thing);

    created_thing.parent_thing = parent_thing_id;
    adjust_thing_pos(new_thing, created_thing.position + parent_look_orient.transform(offset_vec));

    // TODO: Don't orient velocity, let flags handle it.
    created_thing.vel = parent_look_orient.transform(created_thing.vel);

    // TODO: Deal with error vec, autoaim fov.

    if(fire_sound_id >= 0) {
        sound_presenter->play_sound_pos(fire_sound_id, created_thing.position, 1.0f, -1.0f, -1.0f, flag_set<flags::sound_flag>());
    }

    if(puppet_submode_id >= 0) {
        key_presenter->play_mode(parent_thing_id, static_cast<flags::puppet_submode_type>(puppet_submode_id));
    }

    return new_thing;
}

void gorc::game::world::level_presenter::adjust_thing_pos(int thing_id, const vector<3>& new_pos) {
    auto& thing = model->get_thing(thing_id);
    auto old_pos = thing.position;
    thing.position = new_pos;
    update_thing_sector(thing_id, thing, old_pos);
}

void gorc::game::world::level_presenter::set_thing_pos(int thing_id, const vector<3>& new_pos, const quaternion<float>& new_orient, int new_sector) {
    components::thing& thing = model->get_thing(thing_id);
    thing.position = new_pos;
    thing.orient = new_orient;
    thing.sector = new_sector;
}

void gorc::game::world::level_presenter::attach_thing_to_thing(int thing_id, int base_id) {
    auto& thing = model->get_thing(thing_id);
    auto& base = model->get_thing(base_id);

    thing.attach_flags = flag_set<flags::attach_flag> { flags::attach_flag::AttachedToThing };
    thing.attached_thing = base_id;
    thing.prev_attached_thing_position = base.position;
}

int gorc::game::world::level_presenter::create_thing_at_thing(int tpl_id, int thing_id) {
    if(tpl_id < 0) {
        // TODO: No template. Report error.
        return -1;
    }

    components::thing& referencedThing = model->get_thing(thing_id);
    int new_thing_id = create_thing(tpl_id, referencedThing.sector, referencedThing.position, referencedThing.orient);
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

float gorc::game::world::level_presenter::damage_thing(entity_id thing_id,
                                                       float damage,
                                                       flag_set<flags::damage_flag> flags,
                                                       entity_id damager_id) {
    script_presenter->send_message_to_linked(cog::message_id::damaged,
                                             static_cast<int>(thing_id),
                                             flags::message_type::thing,
                                             damager_id,
                                             flags::message_type::thing,
                                             damage,
                                             static_cast<int>(flags));

    components::thing& referencedThing = model->get_thing(thing_id);
    if(referencedThing.health > 0.0f) {
        referencedThing.health -= damage;

        if(referencedThing.health <= 0.0f &&
           (referencedThing.type == flags::thing_type::Player ||
            referencedThing.type == flags::thing_type::Actor)) {
            eventbus->fire_event(events::killed(thing_id, damager_id));
        }
        else {
            eventbus->fire_event(events::class_sound(thing_id,
                                                     flags::sound_subclass_type::HurtSpecial));
            maybe_if(referencedThing.pup, [&](auto) {
                key_presenter->play_mode(thing_id, flags::puppet_submode_type::Hit);
            });
        }
    }

    // TODO: Return actual 'undamaged' value as computed by thing cog with ReturnEx.

    return 0.0f;
}

void gorc::game::world::level_presenter::destroy_thing(int thing_id) {
    things_to_destroy.insert(thing_id);
}

void gorc::game::world::level_presenter::real_destroy_thing(int thing_id) {
    // Reset thing parentage.
    for(auto& thing_pair : model->ecs.all_components<components::thing>()) {
        if(thing_pair.second.parent_thing == thing_id) {
            thing_pair.second.parent_thing = -1;
        }
    }

    // Expunge associated resources.
    key_presenter->expunge_thing_animations(thing_id);

    model->ecs.erase_entity(entity_id(thing_id));
}

void gorc::game::world::level_presenter::detach_thing(int thing_id) {
    model->get_thing(thing_id).attach_flags = flag_set<flags::attach_flag>();
}

gorc::vector<3> gorc::game::world::level_presenter::get_thing_pos(int thing_id) {
    components::thing& referenced_thing = model->get_thing(thing_id);
    return referenced_thing.position;
}

void gorc::game::world::level_presenter::heal_thing(int thing_id, float amount) {
    components::thing& referencedThing = model->get_thing(thing_id);
    referencedThing.health = clamp(referencedThing.health + amount, 0.0f, referencedThing.max_health);
}

bool gorc::game::world::level_presenter::is_thing_moving(int thing_id) {
    // TODO: Temporary hack implementation pending new physics implementation.
    components::thing& referencedThing = model->get_thing(thing_id);
    switch(referencedThing.move) {
    case flags::move_type::physics:
        return length(referencedThing.vel) > 0.0000001f;

    case flags::move_type::Path:
        return referencedThing.path_moving || referencedThing.rotatepivot_moving;

    default:
        return false;
    }
}

gorc::vector<3, float> gorc::game::world::level_presenter::get_thing_lvec(int thing_id) {
    const auto& thing = model->get_thing(thing_id);
    return thing.orient.transform(make_vector(0.0f, 1.0f, 0.0f));
}

// thing flags verbs
void gorc::game::world::level_presenter::clear_actor_flags(int thing_id, flag_set<flags::actor_flag> flags) {
    model->get_thing(thing_id).actor_flags -= flags;
}

gorc::flag_set<gorc::flags::actor_flag> gorc::game::world::level_presenter::get_actor_flags(int thing_id) {
    return model->get_thing(thing_id).actor_flags;
}

void gorc::game::world::level_presenter::set_actor_flags(int thing_id, flag_set<flags::actor_flag> flags) {
    model->get_thing(thing_id).actor_flags += flags;
}

void gorc::game::world::level_presenter::clear_thing_flags(int thing_id, flag_set<flags::thing_flag> flags) {
    model->get_thing(thing_id).flags -= flags;
}

gorc::flag_set<gorc::flags::thing_flag> gorc::game::world::level_presenter::get_thing_flags(int thing_id) {
    return model->get_thing(thing_id).flags;
}

void gorc::game::world::level_presenter::set_thing_flags(int thing_id, flag_set<flags::thing_flag> flags) {
    model->get_thing(thing_id).flags += flags;
}

void gorc::game::world::level_presenter::clear_thing_attach_flags(int thing_id, flag_set<flags::attach_flag> flags) {
    model->get_thing(thing_id).attach_flags -= flags;
}

gorc::flag_set<gorc::flags::attach_flag> gorc::game::world::level_presenter::get_thing_attach_flags(int thing_id) {
    return model->get_thing(thing_id).attach_flags;
}

void gorc::game::world::level_presenter::set_thing_attach_flags(int thing_id, flag_set<flags::attach_flag> flags) {
    model->get_thing(thing_id).attach_flags += flags;
}

void gorc::game::world::level_presenter::jk_clear_flags(int thing_id, flag_set<flags::jk_flag> flags) {
    model->get_thing(thing_id).jk_flags -= flags;
}

gorc::flag_set<gorc::flags::jk_flag> gorc::game::world::level_presenter::jk_get_flags(int thing_id) {
    return model->get_thing(thing_id).jk_flags;
}

void gorc::game::world::level_presenter::jk_set_flags(int thing_id, flag_set<flags::jk_flag> flags) {
    model->get_thing(thing_id).jk_flags += flags;
}

// thing property verbs
int gorc::game::world::level_presenter::get_thing_parent(int thing_id) {
    return model->get_thing(thing_id).attached_thing;
}

int gorc::game::world::level_presenter::get_thing_sector(int thing_id) {
    return model->get_thing(thing_id).sector;
}

gorc::flags::thing_type gorc::game::world::level_presenter::get_thing_type(int thing_id) {
    return model->get_thing(thing_id).type;
}

void gorc::game::world::level_presenter::set_thing_light(int thing_id, float light, float) {
    // TODO: Implement fade_time
    auto& thing = model->get_thing(thing_id);
    thing.light = light;
}

// velocity verbs

void gorc::game::world::level_presenter::stop_thing(int thing) {
    auto& t = model->get_thing(thing);

    t.vel = make_zero_vector<3, float>();
    t.thrust = make_zero_vector<3, float>();
    t.ang_vel = make_zero_vector<3, float>();
    t.rot_thrust = make_zero_vector<3, float>();
}

// weapon verbs

void gorc::game::world::level_presenter::jk_set_weapon_mesh(int player, int mesh) {
    auto& thing = model->get_thing(player);

    if(mesh >= 0) {
        thing.weapon_mesh = contentmanager->get_asset<content::assets::model>(mesh);
    }
    else {
        thing.weapon_mesh = nothing;
    }
}

void gorc::game::world::level_presenter::set_armed_mode(entity_id player, flags::armed_mode mode) {
    eventbus->fire_event(events::armed_mode_changed(player, mode));
}

gorc::flags::puppet_mode_type gorc::game::world::level_presenter::get_major_mode(entity_id player) {
    for(auto const &pup : model->ecs.find_component<components::puppet_animations>(player)) {
        return pup.second.puppet_mode_type;
    }

    return flags::puppet_mode_type::unarmed;
}

void gorc::game::world::level_presenter::register_verbs(cog::verbs::verb_table& verbTable, level_state& components) {

    camera::camera_presenter::register_verbs(verbTable, components);
    animations::animation_presenter::register_verbs(verbTable, components);
    scripts::script_presenter::register_verbs(verbTable, components);
    sounds::sound_presenter::register_verbs(verbTable, components);
    keys::key_presenter::register_verbs(verbTable, components);
    inventory::inventory_presenter::register_verbs(verbTable, components);
    physics::physics_presenter::register_verbs(verbTable, components);

    // AI verbs
    verbTable.add_verb<void, 2>("aiclearmode", [&components](int thing_id, int flags) {
        components.current_level_presenter->ai_clear_mode(thing_id, flag_set<flags::ai_mode_flag>(flags));
    });

    verbTable.add_verb<int, 1>("aigetmode", [&components](int thing_id) {
        return static_cast<int>(components.current_level_presenter->ai_get_mode(thing_id));
    });

    verbTable.add_verb<void, 2>("aisetlookframe", [&components](int thing_id, int frame) {
        components.current_level_presenter->ai_set_look_frame(thing_id, frame);
    });

    verbTable.add_verb<void, 2>("aisetlookpos", [&components](int thing_id, vector<3> pos) {
        components.current_level_presenter->ai_set_look_pos(thing_id, pos);
    });

    verbTable.add_verb<void, 2>("aisetmode", [&components](int thing_id, int flags) {
        components.current_level_presenter->ai_set_mode(thing_id, flag_set<flags::ai_mode_flag>(flags));
    });

    verbTable.add_verb<void, 2>("aisetmoveframe", [&components](int thing_id, int frame) {
        components.current_level_presenter->ai_set_move_frame(thing_id, frame);
    });

    verbTable.add_verb<void, 2>("aisetmovepos", [&components](int thing_id, vector<3> pos) {
        components.current_level_presenter->ai_set_move_pos(thing_id, pos);
    });

    verbTable.add_verb<void, 2>("aisetmovespeed", [&components](int thing_id, float speed) {
        components.current_level_presenter->ai_set_move_speed(thing_id, speed);
    });

    verbTable.add_verb<void, 2>("aisetmovething", [&components](int thing_id, int move_to_thing) {
        components.current_level_presenter->ai_set_move_thing(thing_id, move_to_thing);
    });

    // Color verbs
    verbTable.add_verb<void, 4>("adddynamictint", [&components](int player_id, float r, float g, float b) {
        components.current_level_presenter->add_dynamic_tint(player_id, make_vector(r, g, b));
    });

    // Creature verbs
    verbTable.add_verb<float, 1>("gethealth", [&components](int thing_id) {
        return components.current_level_presenter->get_thing_health(thing_id);
    });

    verbTable.add_verb<float, 1>("getthinghealth", [&components](int thing_id) {
        return components.current_level_presenter->get_thing_health(thing_id);
    });

    verbTable.add_verb<bool, 2>("haslos", [&components](int look_thing, int target) {
        return components.current_level_presenter->has_los(look_thing, target);
    });

    // Frame verbs
    verbTable.add_verb<int, 1>("getcurframe", [&components](entity_id thing) {
        return components.current_level_presenter->get_cur_frame(thing);
    });

    verbTable.add_verb<void, 3>("jumptoframe", [&components](entity_id thing, int frame, int sector) {
        return components.current_level_presenter->jump_to_frame(thing, frame, sector);
    });

    verbTable.add_verb<void, 3>("movetoframe", [&components](entity_id thing, int frame, float speed) {
        return components.current_level_presenter->move_to_frame(thing, frame, speed);
    });

    verbTable.add_verb<void, 1>("pathmovepause", [&components](entity_id thing) {
        components.current_level_presenter->path_move_pause(thing);
    });

    verbTable.add_verb<void, 1>("pathmoveresume", [&components](entity_id thing) {
        components.current_level_presenter->path_move_resume(thing);
    });

    verbTable.add_verb<void, 3>("rotatepivot", [&components](entity_id thing_id, int frame, float time) {
        components.current_level_presenter->rotate_pivot(thing_id, frame, time);
    });

    // level verbs
    verbTable.add_verb<float, 0>("getgametime", [&components] {
        return components.current_level_presenter->get_game_time();
    });

    verbTable.add_verb<float, 0>("getleveltime", [&components] {
        return components.current_level_presenter->get_level_time();
    });

    verbTable.add_verb<void, 1>("jkendlevel", [&components] (bool success) {
        components.current_level_presenter->jk_end_level(success);
    });

    // Misc verbs
    verbTable.add_verb<void, 1>("jkdisablesaber", [&components](int thing_id) {
        components.current_level_presenter->jk_disable_saber(thing_id);
    });

    verbTable.add_verb<void, 4>("jkenablesaber", [&components](int thing_id, float damage,
            float collide_length, float unknown) {
        components.current_level_presenter->jk_enable_saber(thing_id, damage, collide_length, unknown);
    });

    verbTable.add_verb<void, 9>("jksetsaberinfo", [&components](int thing_id,
            int side_mat, int tip_mat, float base_rad, float tip_rad, float length,
            int wall, int blood, int saber) {
        components.current_level_presenter->jk_set_saber_info(thing_id, side_mat, tip_mat,
                base_rad, tip_rad, length, wall, blood, saber);
    });

    verbTable.add_verb<void, 2>("takeitem", [&components](entity_id thing_id, entity_id player_id) {
        components.current_level_presenter->take_item(thing_id, player_id);
    });

    // Options verbs
    verbTable.add_verb<int, 0>("getdifficulty", [] {
        // TODO: Add actual difficulty setting.
        return static_cast<int>(flags::difficulty_mode::medium);
    });

    verbTable.add_verb<int, 0>("getautoswitch", [] {
        // TODO: Add actual autoswitch code.
        return 0x3;
    });

    verbTable.add_verb<int, 0>("getautopickup", [] {
        // TODO: Add actual autopickup code.
        return 0xF;
    });

    verbTable.add_verb<int, 0>("jkgetsabercam", [] {
        // TODO: Add actual sabercam code.
        return 1;
    });

    verbTable.add_verb<int, 0>("ismulti", [] {
        // TODO: Return actual multiplayer state.
        return 0;
    });

    // Player verbs
    verbTable.add_verb<int, 0>("getlocalplayerthing", [&components] {
        return components.current_level_presenter->get_local_player_thing();
    });

    verbTable.add_verb<int, 0>("jkgetlocalplayer", [&components] {
        return components.current_level_presenter->get_local_player_thing();
    });

    // Print verbs:
    verbTable.add_verb<void, 2>("jkprintunistring", [&components](int /*destination*/, int message_num) {
        // TODO: Add actual jkPrintUniString once localization is implemented.
        LOG_INFO(format("COG_%d") % message_num);
    });

    verbTable.add_verb<void, 1>("print", [&components](const char* message) {
        // TODO: Add actual print.
        LOG_INFO(message);
    });

    verbTable.add_verb<void, 1>("printint", [&components](int value) {
        // TODO: Add actual printint.
        LOG_INFO(format("%d") % value);
    });

    // sector verbs
    verbTable.add_verb<void, 2>("clearsectorflags", [&components](int sector_id, int flags) {
        components.current_level_presenter->clear_sector_flags(sector_id, flag_set<flags::sector_flag>(flags));
    });

    verbTable.add_verb<int, 1>("getsectorflags", [&components](int sector_id) {
        return static_cast<int>(components.current_level_presenter->get_sector_flags(sector_id));
    });

    verbTable.add_verb<int, 1>("firstthinginsector", [&components](int sector_id) {
        return components.current_level_presenter->first_thing_in_sector(sector_id);
    });

    verbTable.add_verb<int, 1>("nextthinginsector", [&components](int thing_id) {
        return components.current_level_presenter->next_thing_in_sector(thing_id);
    });

    verbTable.add_verb<void, 2>("sectoradjoins", [&components](int sector_id, bool state) {
        components.current_level_presenter->set_sector_adjoins(sector_id, state);
    });

    verbTable.add_verb<void, 3>("sectorlight", [&components](int sector_id, float light, float delay) {
        components.current_level_presenter->set_sector_light(sector_id, light, delay);
    });

    verbTable.add_verb<void, 3>("sectorsound", [&components](int sector_id, int sound, float volume) {
        components.current_level_presenter->sector_sound(sector_id, sound, volume);
    });

    verbTable.add_verb<void, 3>("sectorthrust", [&components](int sector_id, vector<3> thrust_vec, float thrust_speed) {
        components.current_level_presenter->set_sector_thrust(sector_id, normalize(thrust_vec) * thrust_speed);
    });

    verbTable.add_verb<void, 2>("setcolormap", [&components](int sector_id, int colormap) {
        components.current_level_presenter->set_sector_colormap(sector_id, colormap);
    });

    verbTable.add_verb<void, 2>("setsectoradjoins", [&components](int sector_id, bool state) {
        components.current_level_presenter->set_sector_adjoins(sector_id, state);
    });

    verbTable.add_verb<void, 2>("setsectorcolormap", [&components](int sector_id, int colormap) {
        components.current_level_presenter->set_sector_colormap(sector_id, colormap);
    });

    verbTable.add_verb<void, 2>("setsectorflags", [&components](int sector_id, int flags) {
        components.current_level_presenter->set_sector_flags(sector_id, flag_set<flags::sector_flag>(flags));
    });

    verbTable.add_verb<void, 3>("setsectorlight", [&components](int sector_id, float light, float delay) {
        components.current_level_presenter->set_sector_light(sector_id, light, delay);
    });

    verbTable.add_verb<void, 3>("setsectorthrust", [&components](int sector_id, vector<3> thrust_vec, float thrust_speed) {
        components.current_level_presenter->set_sector_thrust(sector_id, normalize(thrust_vec) * thrust_speed);
    });

    verbTable.add_verb<void, 2>("setsectortint", [&components](int sector_id, vector<3> tint) {
        components.current_level_presenter->set_sector_tint(sector_id, tint);
    });

    // surface verbs
    verbTable.add_verb<void, 2>("clearadjoinflags", [&components](int surface, int flags) {
        components.current_level_presenter->clear_adjoin_flags(surface, flag_set<flags::adjoin_flag>(flags));
    });

    verbTable.add_verb<int, 1>("getfacegeomode", [&components](int surface) {
        return static_cast<int>(components.current_level_presenter->get_face_geo_mode(surface));
    });

    verbTable.add_verb<vector<3>, 1>("getsurfacecenter", [&components](int surface) {
        return components.current_level_presenter->get_surface_center(surface);
    });

    verbTable.add_verb<int, 1>("getsurfacesector", [&components](int surface) {
        return components.current_level_presenter->get_surface_sector(surface);
    });

    verbTable.add_verb<void, 2>("setadjoinflags", [&components](int surface, int flags) {
        components.current_level_presenter->set_adjoin_flags(surface, flag_set<flags::adjoin_flag>(flags));
    });

    verbTable.add_verb<void, 2>("setfacegeomode", [&components](int surface, int mode) {
        components.current_level_presenter->set_face_geo_mode(surface, static_cast<flags::geometry_mode>(mode));
    });

    verbTable.add_verb<void, 2>("setfacetype", [&components](int surface, int flags) {
        components.current_level_presenter->set_face_type(surface, flag_set<flags::face_flag>(flags));
    });

    verbTable.add_verb<void, 2>("setsurfaceflags", [&components](int surface, int flags) {
        components.current_level_presenter->set_surface_flags(surface, flag_set<flags::surface_flag>(flags));
    });

    verbTable.add_verb<void, 2>("setsurfacemat", [&components](int, int) {
        // TODO: Not implemented for now. Architecturally inconvenient.
    });

    verbTable.add_verb<vector<3>, 1>("surfacecenter", [&components](int surface) {
        return components.current_level_presenter->get_surface_center(surface);
    });

    // System verbs
    verbTable.add_verb<int, 1>("loadsound", [&components](const char* fn) {
        return components.current_level_presenter->load_sound(fn);
    });

    // thing action verbs
    verbTable.add_verb<void, 2>("attachthingtothing", [&components](int attach_thing, int base_thing) {
        components.current_level_presenter->attach_thing_to_thing(attach_thing, base_thing);
    });

    verbTable.add_verb<int, 2>("creatething", [&components](int tpl_id, int thing_pos) {
        return components.current_level_presenter->create_thing_at_thing(tpl_id, thing_pos);
    });

    verbTable.add_verb<int, 4>("createthingatpos", [&components](int tpl_id, int sector, vector<3> pos, vector<3> orient) {
        return components.current_level_presenter->create_thing(tpl_id, sector, pos, make_euler(orient));
    });

    verbTable.add_verb<int, 10>("fireprojectile", [&components](entity_id parent_thing_id, int tpl_id, int snd_id, int submode_id,
            vector<3> offset_vec, vector<3> error_vec, float extra, int proj_flags, float aa_fovx, float aa_fovz) {
        return components.current_level_presenter->fire_projectile(parent_thing_id, tpl_id, snd_id, submode_id,
                offset_vec, error_vec, extra, proj_flags, aa_fovx, aa_fovz);
    });

    verbTable.add_verb<float, 4>("damagething", [&components](entity_id thing_id, float damage, int flags, entity_id damager_id) {
        return components.current_level_presenter->damage_thing(thing_id, damage, flag_set<flags::damage_flag>(flags), damager_id);
    });

    verbTable.add_verb<void, 1>("destroything", [&components](int thing_id) {
        components.current_level_presenter->destroy_thing(thing_id);
    });

    verbTable.add_verb<void, 1>("detachthing", [&components](int thing_id) {
        components.current_level_presenter->detach_thing(thing_id);
    });

    verbTable.add_verb<vector<3>, 1>("getthingpos", [&components](int thing_id) {
        return components.current_level_presenter->get_thing_pos(thing_id);
    });

    verbTable.add_verb<void, 2>("healthing", [&components](int thing_id, float amount) {
        components.current_level_presenter->heal_thing(thing_id, amount);
    });

    verbTable.add_verb<bool, 1>("isthingmoving", [&components](int thing_id) {
        return components.current_level_presenter->is_thing_moving(thing_id);
    });

    verbTable.add_verb<bool, 1>("ismoving", [&components](int thing_id) {
        return components.current_level_presenter->is_thing_moving(thing_id);
    });

    verbTable.add_verb<vector<3>, 1>("getthinglvec", [&components](int thing_id) {
        return components.current_level_presenter->get_thing_lvec(thing_id);
    });

    // thing flags verbs
    verbTable.add_verb<void, 2>("clearactorflags", [&components](int thing_id, int flags) {
        components.current_level_presenter->clear_actor_flags(thing_id, flag_set<flags::actor_flag>(flags));
    });

    verbTable.add_verb<int, 1>("getactorflags", [&components](int thing_id) {
        return static_cast<int>(components.current_level_presenter->get_actor_flags(thing_id));
    });

    verbTable.add_verb<void, 2>("setactorflags", [&components](int thing_id, int flags) {
        components.current_level_presenter->set_actor_flags(thing_id, flag_set<flags::actor_flag>(flags));
    });

    verbTable.add_verb<void, 2>("clearthingflags", [&components](int thing_id, int flags) {
        components.current_level_presenter->clear_thing_flags(thing_id, flag_set<flags::thing_flag>(flags));
    });

    verbTable.add_verb<int, 1>("getthingflags", [&components](int thing_id) {
        return static_cast<int>(components.current_level_presenter->get_thing_flags(thing_id));
    });

    verbTable.add_verb<void, 2>("setthingflags", [&components](int thing_id, int flags) {
        components.current_level_presenter->set_thing_flags(thing_id, flag_set<flags::thing_flag>(flags));
    });

    verbTable.add_verb<void, 2>("clearthingattachflags", [&components](int thing_id, int flags) {
        components.current_level_presenter->clear_thing_attach_flags(thing_id, flag_set<flags::attach_flag>(flags));
    });

    verbTable.add_verb<int, 1>("getthingattachflags", [&components](int thing_id) {
        return static_cast<int>(components.current_level_presenter->get_thing_attach_flags(thing_id));
    });

    verbTable.add_verb<void, 2>("setthingattachflags", [&components](int thing_id, int flags) {
        components.current_level_presenter->set_thing_attach_flags(thing_id, flag_set<flags::attach_flag>(flags));
    });

    verbTable.add_verb<void, 2>("jkclearflags", [&components](int thing_id, int flags) {
        components.current_level_presenter->jk_clear_flags(thing_id, flag_set<flags::jk_flag>(flags));
    });

    verbTable.add_verb<int, 1>("jkgetflags", [&components](int thing_id) {
        return static_cast<int>(components.current_level_presenter->jk_get_flags(thing_id));
    });

    verbTable.add_verb<void, 2>("jksetflags", [&components](int thing_id, int flags) {
        components.current_level_presenter->jk_set_flags(thing_id, flag_set<flags::jk_flag>(flags));
    });

    // thing property verbs
    verbTable.add_verb<int, 1>("getthingparent", [&components](int thing_id) {
        return components.current_level_presenter->get_thing_parent(thing_id);
    });

    verbTable.add_verb<int, 1>("getthingsector", [&components](int thing_id) {
        return components.current_level_presenter->get_thing_sector(thing_id);
    });

    verbTable.add_verb<int, 1>("getthingtype", [&components](int thing_id) {
        return static_cast<int>(components.current_level_presenter->get_thing_type(thing_id));
    });

    verbTable.add_verb<void, 3>("setthinglight", [&components](int thing_id, float light, float fade_time) {
        components.current_level_presenter->set_thing_light(thing_id, light, fade_time);
    });

    verbTable.add_verb<void, 3>("thinglight", [&components](int thing_id, float light, float fade_time) {
        components.current_level_presenter->set_thing_light(thing_id, light, fade_time);
    });

    // velocity verbs
    verbTable.add_verb<void, 1>("stopthing", [&components](int thing_id) {
        components.current_level_presenter->stop_thing(thing_id);
    });

    verbTable.add_verb<void, 2>("setthingrotvel", [&components](int thing_id, vector<3> vel) {
        // TODO: Proper implementation
        components.current_level_presenter->model->get_thing(thing_id).ang_vel = vel;
    });

    verbTable.add_verb<void, 2>("applyforce", [&components](int thing_id, vector<3> force) {
        // TODO: Proper implementation
        components.current_level_presenter->model->get_thing(thing_id).vel += force;
    });

    verbTable.add_verb<vector<3>, 1>("getthingthrust", [&components](int thing_id) {
        // TODO: Proper implementation
        return components.current_level_presenter->model->get_thing(thing_id).thrust;
    });

    // weapon verbs
    verbTable.add_verb<void, 2>("jksetweaponmesh", [&components](int thing_id, int mesh_id) {
        components.current_level_presenter->jk_set_weapon_mesh(thing_id, mesh_id);
    });

    verbTable.add_verb<void, 2>("setarmedmode", [&components](entity_id player, int mode) {
        components.current_level_presenter->set_armed_mode(player, flags::armed_mode(mode));
    });

    verbTable.add_verb<int, 1>("getmajormode", [&components](entity_id player) {
        return static_cast<int>(components.current_level_presenter->get_major_mode(player));
    });
}
