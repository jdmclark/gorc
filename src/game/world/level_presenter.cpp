#include "level_presenter.h"
#include "level_model.h"
#include "level_view.h"
#include "game/constants.h"
#include "physics/query.h"

using namespace gorc::math;

gorc::game::world::level_presenter::level_presenter(application& components, const level_place& place)
	: components(components), place(place), contentmanager(place.contentmanager),
	  physics_presenter(*this), script_presenter(components), sound_presenter(*place.contentmanager),
	  key_presenter(*place.contentmanager), inventory_presenter(*this), camera_presenter(*this),
	  actor_controller(*this), player_controller(*this), cog_controller(*this), ghost_controller(*this),
	  item_controller(*this), corpse_controller(*this), weapon_controller(*this) {

	return;
}

void gorc::game::world::level_presenter::start(event::event_bus& eventBus) {
	model = std::unique_ptr<level_model>(new level_model(*place.contentmanager, components.compiler, place.level,
			place.contentmanager->load<content::assets::inventory>("items.dat", components.compiler)));

	physics_presenter.start(*model);
	key_presenter.start(*model, model->key_model);
	camera_presenter.start(*model, model->camera_model);
	animation_presenter.start(*model, model->animation_model);
	script_presenter.start(*model, model->script_model);
	sound_presenter.start(*model, model->sound_model);
	inventory_presenter.start(*model, model->inventory_model);

	initialize_world();

	components.level_view->set_presenter(this);
	components.level_view->set_level_model(model.get());

	components.views.set_layer(view_layer::world, *components.level_view);

	// Update all components
	update(time(0, 0.0));

	// Send startup and loading messages
	script_presenter.send_message_to_all(cog::message_id::loading, -1, -1, flags::message_type::nothing);

	return;
}

void gorc::game::world::level_presenter::initialize_world() {
	script_presenter.create_level_dummy_instances(model->level.cogs.size());

	// HACK: create thing collision shapes and rigid bodies, enumerate spawn points
	for(const auto& thing : model->level.things) {
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
	camera_presenter.set_camera_focus(0, model->local_player_thing_id);
	camera_presenter.set_camera_focus(1, model->local_player_thing_id);

	// create bin script instances.
	for(const auto& bin_tuple : model->inventory_model.base_inventory) {
		const auto& bin = std::get<1>(bin_tuple);

		if(bin.cog) {
			script_presenter.create_global_cog_instance(bin.cog->cogscript, *place.contentmanager, components.compiler);
		}
	}

	// create COG script instances.
	for(unsigned int i = 0; i < model->level.cogs.size(); ++i) {
		const auto& cog = model->level.cogs[i];
		content::assets::script const* script = std::get<0>(cog);
		const std::vector<cog::vm::value>& values = std::get<1>(cog);

		if(script) {
			script_presenter.create_level_cog_instance(i, script->cogscript, *place.contentmanager, components.compiler, values);
		}
	}
}

void gorc::game::world::level_presenter::update(const time& time) {
	double dt = time;

	physics_presenter.update(time);
	camera_presenter.update(time);
	animation_presenter.update(time);
	script_presenter.update(time);
	sound_presenter.update(time);
	key_presenter.update(time);
	inventory_presenter.update(time);

	// update things
	for(auto& thing : model->things) {
		thing.controller->update(thing.get_id(), dt);
	}

	// update dynamic tint, game time.
	model->game_time += dt;
	model->level_time += dt;

	model->dynamic_tint = model->dynamic_tint * (1.0 - dt);
	get<0>(model->dynamic_tint) = std::max(get<0>(model->dynamic_tint), 0.0f);
	get<1>(model->dynamic_tint) = std::max(get<1>(model->dynamic_tint), 0.0f);
	get<2>(model->dynamic_tint) = std::max(get<2>(model->dynamic_tint), 0.0f);
}

gorc::game::world::gameplay::thing_controller& gorc::game::world::level_presenter::get_thing_controller(flags::thing_type type) {
	switch(type) {
	case flags::thing_type::Actor:
		return actor_controller;

	case flags::thing_type::cog:
		return cog_controller;

	case flags::thing_type::Player:
		return player_controller;

	case flags::thing_type::Corpse:
		return corpse_controller;

	case flags::thing_type::Item:
		return item_controller;

	case flags::thing_type::Weapon:
		return weapon_controller;

	default:
	case flags::thing_type::ghost:
		return ghost_controller;
	}
}

void gorc::game::world::level_presenter::update_thing_sector(int thing_id, thing& thing,
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
		script_presenter.send_message_to_linked(cog::message_id::crossed, first_adjoin, flags::message_type::surface,
				static_cast<int>(thing_id), flags::message_type::thing);
	}

	for(unsigned int i = 1; i < update_path_sector_scratch.size() - 1; ++i) {
		unsigned int sec_id = std::get<0>(update_path_sector_scratch[i]);
		thing.sector = sec_id;
		if(model->sectors[sec_id].flags & flags::sector_flag::CogLinked) {
			script_presenter.send_message_to_linked(cog::message_id::entered, sec_id, flags::message_type::sector,
					static_cast<int>(thing_id), flags::message_type::thing);
		}

		unsigned int surf_id = std::get<1>(update_path_sector_scratch[i]);
		if(model->surfaces[surf_id].flags & flags::surface_flag::CogLinked) {
			script_presenter.send_message_to_linked(cog::message_id::crossed, surf_id, flags::message_type::surface,
					static_cast<int>(thing_id), flags::message_type::thing);
		}
	}

	unsigned int last_sector = std::get<0>(update_path_sector_scratch.back());
	thing.sector = last_sector;
	if(model->sectors[last_sector].flags & flags::sector_flag::CogLinked) {
		script_presenter.send_message_to_linked(cog::message_id::entered, last_sector, flags::message_type::sector,
				static_cast<int>(thing_id), flags::message_type::thing);
	}
}

void gorc::game::world::level_presenter::translate_camera(const vector<3>& amt) {
	auto& player = model->things[model->local_player_thing_id];
	player.thrust = orient_direction_vector(amt, make_vector(0.0f, get<1>(player.orient), 0.0f));
}

void gorc::game::world::level_presenter::yaw_camera(double amt) {
	auto& player = model->things[model->local_player_thing_id];
	get<1>(player.ang_vel) = amt * 60.0f;
	//get<1>(player.orient) += amt;
}

void gorc::game::world::level_presenter::pitch_camera(double amt) {
	auto& player = model->things[model->local_player_thing_id];
	player.head_pitch = clamp(player.head_pitch + static_cast<float>(amt), player.min_head_pitch, player.max_head_pitch);
}

void gorc::game::world::level_presenter::respawn() {
	++model->current_spawn_point;
	model->current_spawn_point = model->current_spawn_point % model->spawn_points.size();

	thing& cameraThing = model->things[model->local_player_thing_id];
	cameraThing.sector = model->spawn_points[model->current_spawn_point]->sector;
	cameraThing.position = model->spawn_points[model->current_spawn_point]->position;
	cameraThing.attach_flags = flag_set<flags::attach_flag>();
	cameraThing.vel = make_zero_vector<3, float>();
}

void gorc::game::world::level_presenter::jump() {
	auto& player = model->things[model->local_player_thing_id];
	get<2>(player.thrust) = player.jump_speed;
}

void gorc::game::world::level_presenter::activate() {
	// TODO: Implement actual surface and thing activation

	auto& player = model->things[model->local_player_thing_id];
	auto player_look_orient = make_vector(player.head_pitch, get<1>(player.orient), get<2>(player.orient));
	vector<3> camera_position = player.position + orient_direction_vector(player.eye_offset, player_look_orient);
	auto camera_look = orient_direction_vector(make_vector(0.0f, 1.0f, 0.0f), player_look_orient);

	int best_surf_candidate = -1;
	float best_surf_dist = 0.25f;

	int best_thing_candidate = -1;
	float best_thing_dist = 0.25f;

	for(size_t i = 0; i < model->surfaces.size(); ++i) {
		const content::assets::level_surface& surf = model->surfaces[i];
		if((surf.adjoin >= 0 && (model->adjoins[surf.adjoin].flags & flags::adjoin_flag::AllowMovement))
				|| !(surf.flags & flags::surface_flag::CogLinked)
				|| dot(surf.normal, camera_look) >= 0.0f) {
			continue;
		}

		for(const auto& vx : surf.vertices) {
			float new_dist = length(camera_position - model->level.vertices[std::get<0>(vx)]);
			if(new_dist < best_surf_dist) {
				best_surf_candidate = i;
				best_surf_dist = new_dist;
				break;
			}
		}
	}

	for(auto& thing : model->things) {
		if(thing.get_id() == model->local_player_thing_id) {
			continue;
		}

		auto dir_vec = thing.position - camera_position;
		if(!(thing.flags & flags::thing_flag::CogLinked) || dot(dir_vec, camera_look) <= 0.0f) {
			continue;
		}

		float dir_len = length(dir_vec);
		if(dir_len >= best_thing_dist) {
			continue;
		}

		best_thing_candidate = thing.get_id();
		best_thing_dist = dir_len;
	}

	if(best_surf_candidate >= 0 && best_surf_dist <= best_thing_dist) {
		script_presenter.send_message_to_linked(cog::message_id::activated, best_surf_candidate, flags::message_type::surface,
				model->local_player_thing_id, flags::message_type::thing);
	}
	else if(best_thing_candidate >= 0) {
		script_presenter.send_message_to_linked(cog::message_id::activated, best_thing_candidate, flags::message_type::thing,
				model->local_player_thing_id, flags::message_type::thing);
		sound_presenter.play_sound_class(best_thing_candidate, flags::sound_subclass_type::Activate);
	}
}

void gorc::game::world::level_presenter::damage() {
	// TODO: Temporary code to fire a bryar bolt.

	// Calculate orientation from camera look.
	const auto& player = model->things[model->local_player_thing_id];

	float bolt_yaw = get<1>(player.orient);
	float bolt_pitch = player.head_pitch;

	int bolt_thing = create_thing("+bryarbolt", player.sector, player.position, make_vector(bolt_pitch, bolt_yaw, 0.0f));

	const auto& bolt_thing_ref = model->things[bolt_thing];
	vector<3> bolt_offset = player.position +
			orient_direction_vector(make_vector(0.0f, 1.0f, 0.0f), make_vector(bolt_pitch, bolt_yaw, 0.0f)) * (player.size * 2.0f + bolt_thing_ref.size);

	adjust_thing_pos(bolt_thing, bolt_offset);

	auto& thing = model->things[bolt_thing];

	// Rotate velocity.
	thing.vel = orient_direction_vector(thing.vel, make_vector(bolt_pitch, bolt_yaw, 0.0f));
}

void gorc::game::world::level_presenter::crouch(bool is_crouched) {
	auto& player = model->things[model->local_player_thing_id];
	if(is_crouched) {
		player.physics_flags += flags::physics_flag::is_crouching;
	}
	else {
		player.physics_flags -= flags::physics_flag::is_crouching;
	}
}

void gorc::game::world::level_presenter::thing_sighted(int thing_id) {
	model->things[thing_id].flags += flags::thing_flag::Sighted;
	script_presenter.send_message_to_linked(cog::message_id::sighted, thing_id, flags::message_type::thing);
}

// Color verbs
void gorc::game::world::level_presenter::add_dynamic_tint(int player_id, const vector<3>& tint) {
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
	return model->things[thing_id].health;
}

// Frame verbs
int gorc::game::world::level_presenter::get_cur_frame(int thing_id) {
	return model->things[thing_id].current_frame;
}

void gorc::game::world::level_presenter::jump_to_frame(int thing_id, int frame, int sector) {
	thing& referenced_thing = model->things[thing_id];
	auto& referenced_frame = referenced_thing.frames[frame];
	set_thing_pos(thing_id, std::get<0>(referenced_frame), std::get<1>(referenced_frame), sector);
}

void gorc::game::world::level_presenter::move_to_frame(int thing_id, int frame, float speed) {
	thing& referenced_thing = model->things[thing_id];

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
	sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::StartMove);
	sound_presenter.play_foley_loop_class(thing_id, flags::sound_subclass_type::Moving);
}

// level verbs
float gorc::game::world::level_presenter::get_game_time() {
	return model->game_time;
}

float gorc::game::world::level_presenter::get_level_time() {
	return model->level_time;
}

// Misc verbs
void gorc::game::world::level_presenter::take_item(int thing_id, int player_id) {
	auto& thing = model->things[thing_id];
	thing.controller->taken(thing_id, player_id);
}

// Player verbs
int gorc::game::world::level_presenter::get_local_player_thing() {
	return model->local_player_thing_id;
}

// sector verbs
void gorc::game::world::level_presenter::set_sector_adjoins(int sector_id, bool state) {
	content::assets::level_sector& sector = model->sectors[sector_id];
	for(unsigned int i = 0; i < sector.surface_count; ++i) {
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
	sector.cmp = make_maybe(&place.contentmanager->get_asset<content::assets::colormap>(colormap));
}

void gorc::game::world::level_presenter::set_sector_light(int sector_id, float value, float delay) {
	// TODO: create animation to implement delay feature.
	content::assets::level_sector& sector = model->sectors[sector_id];
	sector.extra_light = value;
}

void gorc::game::world::level_presenter::set_sector_thrust(int sector_id, const vector<3>& thrust) {
	content::assets::level_sector& sector = model->sectors[sector_id];
	sector.thrust = thrust * rate_factor;
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

gorc::vector<3> gorc::game::world::level_presenter::get_surface_center(int surface) {
	auto vec = make_zero_vector<3, float>();
	for(const auto& vx : model->level.surfaces[surface].vertices) {
		vec += model->level.vertices[std::get<0>(vx)];
	}

	vec = vec / static_cast<float>(model->level.surfaces[surface].vertices.size());
	return vec;
}

void gorc::game::world::level_presenter::set_adjoin_flags(int surface, flag_set<flags::adjoin_flag> flags) {
	content::assets::level_surface& surf = model->surfaces[surface];
	if(surf.adjoin >= 0) {
		content::assets::level_adjoin& adj = model->adjoins[surf.adjoin];
		adj.flags += flags;
	}
}

void gorc::game::world::level_presenter::set_face_geo_mode(int surface, flags::geometry_mode geo_mode) {
	model->surfaces[surface].geometry_mode = geo_mode;
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
		const vector<3>& pos, const vector<3>& orient) {
	// Initialize thing properties
	auto& new_thing = model->things.emplace(tpl);

	new_thing.object_data.thing_id = new_thing.get_id();
	new_thing.sector = sector_num;
	new_thing.position = pos;
	new_thing.orient = orient;
	new_thing.controller = &get_thing_controller(new_thing.type);

	new_thing.controller->create_controller_data(new_thing.get_id());

	if(new_thing.cog) {
		script_presenter.create_global_cog_instance(new_thing.cog->cogscript, *place.contentmanager, components.compiler);
	}

	sound_presenter.play_sound_class(new_thing.get_id(), flags::sound_subclass_type::create);

	return new_thing.get_id();
}

int gorc::game::world::level_presenter::create_thing(int tpl_id, unsigned int sector_num,
		const vector<3>& pos, const vector<3>& orientation) {
	return create_thing(model->level.templates[tpl_id], sector_num, pos, orientation);
}

int gorc::game::world::level_presenter::create_thing(const std::string& tpl_name, unsigned int sector_num,
		const vector<3>& pos, const vector<3>& orientation) {
	std::string temp;
	std::transform(tpl_name.begin(), tpl_name.end(), std::back_inserter(temp), tolower);
	auto it = model->level.template_map.find(temp);
	if(it != model->level.template_map.end()) {
		return create_thing(it->second, sector_num, pos, orientation);
	}
	else {
		// TODO: thing_template not found. report error.
		return -1;
	}
}

int gorc::game::world::level_presenter::fire_projectile(int parent_thing_id, int tpl_id, int fire_sound_id, int puppet_submode_id,
		const vector<3>& offset_vec, const vector<3>& error_vec, float extra, int projectile_flags, float autoaim_fovx, float autoaim_fovy) {
	const auto& parent_thing = model->things[parent_thing_id];

	const auto& parent_look_orient = make_vector(parent_thing.head_pitch, get<1>(parent_thing.orient), get<2>(parent_thing.orient)) + error_vec;

	int new_thing = create_thing(tpl_id, parent_thing.sector, parent_thing.position, parent_look_orient);
	auto& created_thing = model->things[new_thing];

	created_thing.parent_thing = parent_thing_id;
	adjust_thing_pos(new_thing, created_thing.position + orient_direction_vector(offset_vec, parent_look_orient));

	// TODO: Don't orient velocity, let flags handle it.
	created_thing.vel = orient_direction_vector(created_thing.vel, parent_look_orient);

	// TODO: Deal with error vec, autoaim fov.

	if(fire_sound_id >= 0) {
		sound_presenter.play_sound_pos(fire_sound_id, created_thing.position, 1.0f, -1.0f, -1.0f, flag_set<flags::sound_flag>());
	}

	if(puppet_submode_id >= 0) {
		key_presenter.play_mode(parent_thing_id, static_cast<flags::puppet_submode_type>(puppet_submode_id));
	}

	return new_thing;
}

void gorc::game::world::level_presenter::adjust_thing_pos(int thing_id, const vector<3>& new_pos) {
	auto& thing = model->things[thing_id];
	auto old_pos = thing.position;
	thing.position = new_pos;
	update_thing_sector(thing_id, thing, old_pos);
}

void gorc::game::world::level_presenter::set_thing_pos(int thing_id, const vector<3>& new_pos, const vector<3>& new_orient, int new_sector) {
	thing& thing = model->things[thing_id];
	thing.position = new_pos;
	thing.orient = new_orient;
	thing.sector = new_sector;
}

void gorc::game::world::level_presenter::attach_thing_to_thing(int thing_id, int base_id) {
	auto& thing = model->things[thing_id];
	auto& base = model->things[base_id];

	thing.attach_flags = flag_set<flags::attach_flag> { flags::attach_flag::AttachedToThing };
	thing.attached_thing = base_id;
	thing.prev_attached_thing_position = base.position;
}

int gorc::game::world::level_presenter::create_thing_at_thing(int tpl_id, int thing_id) {
	thing& referencedThing = model->things[thing_id];
	int new_thing_id = create_thing(tpl_id, referencedThing.sector, referencedThing.position, referencedThing.orient);
	thing& new_thing = model->things[new_thing_id];

	new_thing.path_moving = false;

	if(new_thing.model_3d) {
		adjust_thing_pos(new_thing_id, new_thing.position + new_thing.model_3d->insert_offset);
	}

	// CreateThingAtThing really does copy frames.
	std::transform(referencedThing.frames.begin(), referencedThing.frames.end(), std::back_inserter(new_thing.frames),
			[&new_thing](const std::tuple<vector<3>, vector<3>>& frame) -> std::tuple<vector<3>, vector<3>> {
		if(new_thing.model_3d) {
			return std::make_tuple(std::get<0>(frame) + new_thing.model_3d->insert_offset, std::get<1>(frame));
		}
		else {
			return frame;
		}
	});

	return new_thing_id;
}

float gorc::game::world::level_presenter::damage_thing(int thing_id, float damage, flag_set<flags::damage_flag> flags, int damager_id) {
	script_presenter.send_message_to_linked(cog::message_id::damaged, static_cast<int>(thing_id), flags::message_type::thing,
			damager_id, flags::message_type::thing, damage, static_cast<int>(flags));

	thing& referencedThing = model->things[thing_id];
	if(referencedThing.health > 0.0f) {
		referencedThing.health -= damage;

		if(referencedThing.health <= 0.0f && (referencedThing.type == flags::thing_type::Actor || referencedThing.type == flags::thing_type::Player)) {
			sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::Death1);
			script_presenter.send_message_to_linked(cog::message_id::killed, static_cast<int>(thing_id), flags::message_type::thing,
					damager_id, flags::message_type::thing);
			// TODO: thing is dead. Reset to corpse
			set_thing_type(thing_id, flags::thing_type::Corpse);
			if(referencedThing.pup) {
				key_presenter.play_mode(thing_id, flags::puppet_submode_type::Death);
			}
		}
		else {
			sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::HurtSpecial);
			if(referencedThing.pup) {
				key_presenter.play_mode(thing_id, flags::puppet_submode_type::Hit);
			}
		}
	}

	// TODO: Return actual 'undamaged' value as computed by thing cog with ReturnEx.

	return 0.0f;
}

void gorc::game::world::level_presenter::destroy_thing(int thing_id) {
	auto& thing = model->things[thing_id];

	// Reset thing parentage.
	for(auto& thing : model->things) {
		if(thing.parent_thing == thing_id) {
			thing.parent_thing = -1;
		}
	}

	// Remove controller data
	model->things[thing_id].controller->remove_controller_data(thing_id);

	// Expunge associated resources.
	sound_presenter.expunge_thing_sounds(thing_id);
	key_presenter.expunge_thing_animations(thing_id);

	model->things.erase(thing_id);
}

void gorc::game::world::level_presenter::detach_thing(int thing_id) {
	model->things[thing_id].attach_flags = flag_set<flags::attach_flag>();
}

gorc::vector<3> gorc::game::world::level_presenter::get_thing_pos(int thing_id) {
	thing& referenced_thing = model->things[thing_id];
	return referenced_thing.position;
}

bool gorc::game::world::level_presenter::is_thing_moving(int thing_id) {
	// TODO: Temporary hack implementation pending new physics implementation.
	thing& referencedThing = model->things[thing_id];
	switch(referencedThing.move) {
	case flags::move_type::physics:
		return true;

	case flags::move_type::Path:
	default:
		return false;
	}
}

// thing flags verbs
void gorc::game::world::level_presenter::clear_actor_flags(int thing_id, flag_set<flags::actor_flag> flags) {
	model->things[thing_id].actor_flags -= flags;
}

void gorc::game::world::level_presenter::clear_thing_flags(int thing_id, flag_set<flags::thing_flag> flags) {
	model->things[thing_id].flags -= flags;
}

gorc::flag_set<gorc::flags::actor_flag> gorc::game::world::level_presenter::get_actor_flags(int thing_id) {
	return model->things[thing_id].actor_flags;
}

void gorc::game::world::level_presenter::jk_clear_flags(int thing_id, flag_set<flags::jk_flag> flags) {
	model->things[thing_id].jk_flags -= flags;
}

void gorc::game::world::level_presenter::set_actor_flags(int thing_id, flag_set<flags::actor_flag> flags) {
	model->things[thing_id].actor_flags += flags;
}

// thing property verbs
int gorc::game::world::level_presenter::get_thing_parent(int thing_id) {
	return model->things[thing_id].attached_thing;
}

int gorc::game::world::level_presenter::get_thing_sector(int thing_id) {
	return model->things[thing_id].sector;
}

gorc::flags::thing_type gorc::game::world::level_presenter::get_thing_type(int thing_id) {
	return model->things[thing_id].type;
}

void gorc::game::world::level_presenter::set_thing_type(int thing_id, flags::thing_type type) {
	// Clean up type physics.
	auto& thing = model->things[thing_id];
	thing.controller->remove_controller_data(thing_id);

	thing.type = type;

	// Install new controller
	thing.controller = &get_thing_controller(thing.type);
	thing.controller->create_controller_data(thing_id);
}

void gorc::game::world::level_presenter::set_thing_light(int thing_id, float light, float fade_time) {
	// TODO: Implement fade_time
	auto& thing = model->things[thing_id];
	thing.light = light;
}

// weapon verbs

void gorc::game::world::level_presenter::jk_set_weapon_mesh(int player, int mesh) {
	auto& thing = model->things[player];

	if(mesh >= 0) {
		thing.weapon_mesh = &contentmanager->get_asset<content::assets::model>(mesh);
	}
	else {
		thing.weapon_mesh = nullptr;
	}
}

void gorc::game::world::level_presenter::set_armed_mode(int player, flags::armed_mode mode) {
	model->things[player].armed_mode = mode;
}

void gorc::game::world::level_presenter::register_verbs(cog::verbs::verb_table& verbTable, application& components) {
	camera::camera_presenter::register_verbs(verbTable, components);
	animations::animation_presenter::register_verbs(verbTable, components);
	scripts::script_presenter::register_verbs(verbTable, components);
	sounds::sound_presenter::register_verbs(verbTable, components);
	keys::key_presenter::register_verbs(verbTable, components);
	inventory::inventory_presenter::register_verbs(verbTable, components);
	physics::physics_presenter::register_verbs(verbTable, components);

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

	// Frame verbs
	verbTable.add_verb<int, 1>("getcurframe", [&components](int thing) {
		return components.current_level_presenter->get_cur_frame(thing);
	});

	verbTable.add_verb<void, 3>("jumptoframe", [&components](int thing, int frame, int sector) {
		return components.current_level_presenter->jump_to_frame(thing, frame, sector);
	});

	verbTable.add_verb<void, 3>("movetoframe", [&components](int thing, int frame, float speed) {
		return components.current_level_presenter->move_to_frame(thing, frame, speed);
	});

	// level verbs
	verbTable.add_verb<float, 0>("getgametime", [&components] {
		return components.current_level_presenter->get_game_time();
	});

	verbTable.add_verb<float, 0>("getleveltime", [&components] {
		return components.current_level_presenter->get_level_time();
	});

	// Misc verbs
	verbTable.add_verb<void, 2>("takeitem", [&components](int thing_id, int player_id) {
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

	// Print verbs
	verbTable.add_verb<void, 2>("jkprintunistring", [&components](int destination, int message_num) {
		// TODO: Add actual jkPrintUniString once localization is implemented.
		std::cout << "COG_" << message_num << std::endl;
	});

	verbTable.add_verb<void, 1>("print", [&components](const char* message) {
		// TODO: Add actual print.
		std::cout << message << std::endl;
	});

	verbTable.add_verb<void, 1>("printint", [&components](int value) {
		// TOOD: Add actual printint.
		std::cout << value << std::endl;
	});

	// sector verbs
	verbTable.add_verb<void, 2>("sectoradjoins", [&components](int sector_id, bool state) {
		components.current_level_presenter->set_sector_adjoins(sector_id, state);
	});

	verbTable.add_verb<void, 3>("sectorlight", [&components](int sector_id, float light, float delay) {
		components.current_level_presenter->set_sector_light(sector_id, light, delay);
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

	verbTable.add_verb<vector<3>, 1>("getsurfacecenter", [&components](int surface) {
		return components.current_level_presenter->get_surface_center(surface);
	});

	verbTable.add_verb<void, 2>("setadjoinflags", [&components](int surface, int flags) {
		components.current_level_presenter->set_adjoin_flags(surface, flag_set<flags::adjoin_flag>(flags));
	});

	verbTable.add_verb<void, 2>("setfacegeomode", [&components](int surface, int mode) {
		components.current_level_presenter->set_face_geo_mode(surface, static_cast<flags::geometry_mode>(mode));
	});

	verbTable.add_verb<void, 2>("setsurfaceflags", [&components](int surface, int flags) {
		components.current_level_presenter->set_surface_flags(surface, flag_set<flags::surface_flag>(flags));
	});

	verbTable.add_verb<vector<3>, 1>("surfacecenter", [&components](int surface) {
		return components.current_level_presenter->get_surface_center(surface);
	});

	// System verbs
	verbTable.add_verb<int, 2>("bittest", [](int flag1, int flag2) {
		return flag1 & flag2;
	});

	verbTable.add_verb<int, 1>("loadsound", [&components](const char* fn) {
		return components.current_level_presenter->load_sound(fn);
	});

	verbTable.add_verb<float, 0>("rand", [&components]{ return static_cast<float>(static_cast<double>(components.randomizer)); });

	// thing action verbs
	verbTable.add_verb<void, 2>("attachthingtothing", [&components](int attach_thing, int base_thing) {
		components.current_level_presenter->attach_thing_to_thing(attach_thing, base_thing);
	});

	verbTable.add_verb<int, 2>("creatething", [&components](int tpl_id, int thing_pos) {
		return components.current_level_presenter->create_thing_at_thing(tpl_id, thing_pos);
	});

	verbTable.add_verb<int, 10>("fireprojectile", [&components](int parent_thing_id, int tpl_id, int snd_id, int submode_id,
			vector<3> offset_vec, vector<3> error_vec, float extra, int proj_flags, float aa_fovx, float aa_fovz) {
		return components.current_level_presenter->fire_projectile(parent_thing_id, tpl_id, snd_id, submode_id,
				offset_vec, error_vec, extra, proj_flags, aa_fovx, aa_fovz);
	});

	verbTable.add_verb<float, 4>("damagething", [&components](int thing_id, float damage, int flags, int damager_id) {
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

	verbTable.add_verb<bool, 1>("isthingmoving", [&components](int thing_id) {
		return components.current_level_presenter->is_thing_moving(thing_id);
	});

	verbTable.add_verb<bool, 1>("ismoving", [&components](int thing_id) {
		return components.current_level_presenter->is_thing_moving(thing_id);
	});

	// thing flags verbs
	verbTable.add_verb<void, 2>("clearactorflags", [&components](int thing_id, int flags) {
		components.current_level_presenter->clear_actor_flags(thing_id, flag_set<flags::actor_flag>(flags));
	});

	verbTable.add_verb<void, 2>("clearthingflags", [&components](int thing_id, int flags) {
		components.current_level_presenter->clear_thing_flags(thing_id, flag_set<flags::thing_flag>(flags));
	});

	verbTable.add_verb<int, 1>("getactorflags", [&components](int thing_id) {
		return static_cast<int>(components.current_level_presenter->get_actor_flags(thing_id));
	});

	verbTable.add_verb<void, 2>("jkclearflags", [&components](int thing_id, int flags) {
		components.current_level_presenter->jk_clear_flags(thing_id, flag_set<flags::jk_flag>(flags));
	});

	verbTable.add_verb<void, 2>("setactorflags", [&components](int thing_id, int flags) {
		components.current_level_presenter->set_actor_flags(thing_id, flag_set<flags::actor_flag>(flags));
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

	// vector verbs
	verbTable.add_verb<vector<3>, 0>("randvec", [&components] {
		return make_vector<float>(components.randomizer * 2.0 - 1.0, components.randomizer * 2.0 - 1.0, components.randomizer * 2.0 - 1.0);
	});

	verbTable.add_verb<vector<3>, 2>("vectorscale", [](vector<3> vec, float fac) {
		return vec * fac;
	});

	verbTable.add_verb<vector<3>, 3>("vectorset", [](float x, float y, float z) {
		return make_vector(x, y, z);
	});

	// weapon verbs
	verbTable.add_verb<void, 2>("jksetweaponmesh", [&components](int thing_id, int mesh_id) {
		components.current_level_presenter->jk_set_weapon_mesh(thing_id, mesh_id);
	});

	verbTable.add_verb<void, 2>("setarmedmode", [&components](int player, int mode) {
		components.current_level_presenter->set_armed_mode(player, flags::armed_mode(mode));
	});
}
