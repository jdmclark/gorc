#include "cog/verbs/table.h"
#include "game/application.h"
#include "camera_presenter.h"
#include "game/world/level_presenter.h"
#include "framework/math/util.h"
#include "camera_model.h"
#include "game/world/level_model.h"
#include "game/world/physics/query.h"

gorc::game::world::camera::camera_presenter::camera_presenter(level_presenter& presenter)
	: presenter(presenter), levelmodel(nullptr), model(nullptr) {
	return;
}

void gorc::game::world::camera::camera_presenter::start(level_model& levelmodel, camera_model& model) {
	this->levelmodel = &levelmodel;
	this->model = &model;

	// Initialize default camera state
	model.state_flags = flag_set<flags::camera_state_flag>();
	model.current_camera = 0;

	auto& internal_camera = model.cameras[0];
	internal_camera.draw_focus = false;
	internal_camera.draw_pov_model = true;

	auto& external_camera = model.cameras[1];
	external_camera.draw_focus = true;
	external_camera.draw_pov_model = false;
	external_camera.base_offset = make_vector(0.0f, -0.2f, 0.0125f);

	// Create POV animation key mix.
	model.pov_key_mix_id = presenter.key_presenter.create_key_mix();
}

void gorc::game::world::camera::camera_presenter::update(const time& time) {
	auto& selected_camera = model->cameras[model->current_camera];
	auto& cam = model->current_computed_state;

	const auto& focus_thing = levelmodel->things[selected_camera.focus];
	const auto focus_thing_orient = focus_thing.orient
			* make_euler(selected_camera.angle_offset)
			* make_rotation(make_vector(1.0f, 0.0f, 0.0f), focus_thing.head_pitch);

	auto true_offset = focus_thing_orient.transform(focus_thing.eye_offset + selected_camera.base_offset + selected_camera.pos_offset);

	// Cast offset vector into world to detect collisions.
	maybe<physics::contact> contact;
	contact = presenter.physics_presenter.thing_segment_query(selected_camera.focus, true_offset, contact);

	auto true_desired_position = focus_thing.position + true_offset;
	if_set(contact, then_do, [&true_desired_position, &focus_thing](const physics::contact& cnt) {
		// Bring back by some small radius - 0.02?
		true_desired_position = cnt.position + normalize(focus_thing.position - cnt.position) * 0.02f;
	});

	// Calculate sector containing camera
	physics::segment_adjoin_path(physics::segment(focus_thing.position, true_desired_position), *levelmodel,
			levelmodel->sectors[focus_thing.sector], update_path_sector_scratch);
	cam.containing_sector = std::get<0>(update_path_sector_scratch.back());
	cam.position = true_desired_position;

	// Jiggle look and up by pov shake.
	cam.look = focus_thing_orient.transform(make_vector(0.0f, 1.0f, 0.0f));
	cam.up = focus_thing_orient.transform(make_vector(0.0f, 0.0f, 1.0f));

	if(selected_camera.draw_focus) {
		cam.focus_not_drawn_thing = -1;
	}
	else {
		cam.focus_not_drawn_thing = selected_camera.focus;
	}

	cam.draw_pov_model = selected_camera.draw_pov_model;

	// Calculate pov model waggle.
	const auto& player_thing = levelmodel->things[presenter.get_local_player_thing()];
	auto player_speed = length(make_vector(get<0>(player_thing.vel), get<1>(player_thing.vel))) / player_thing.max_vel;
	float actual_waggle_speed = player_speed * model->waggle_speed * time * (1.0f / 60.0f);
	model->waggle_time += actual_waggle_speed;
	float waggle_up = -cos(model->waggle_time * 2.0);
	float waggle_left = sin(model->waggle_time);
	auto new_offset = player_speed * make_vector(waggle_up * get<0>(model->waggle) * (9.0f / 16.0f),
			waggle_left * get<1>(model->waggle) * (16.0f / 9.0f), waggle_up * get<2>(model->waggle));
	cam.pov_model_offset = lerp(cam.pov_model_offset, new_offset, 0.1f);

	for(auto& camera_state : model->cameras) {
		camera_state.angle_offset = camera_state.angle_offset - clamp_length(camera_state.angle_offset, 0.0f, camera_state.angle_reset_speed * static_cast<float>(time));
		camera_state.pos_offset = camera_state.pos_offset - clamp_length(camera_state.pos_offset, 0.0f, camera_state.pos_reset_speed * static_cast<float>(time));
	}
}

void gorc::game::world::camera::camera_presenter::cycle_camera() {
	set_current_camera((model->current_camera + 1) % 2);
}

int gorc::game::world::camera::camera_presenter::get_camera_state_flags() {
	return static_cast<int>(model->state_flags);
}

int gorc::game::world::camera::camera_presenter::get_current_camera() {
	return model->current_camera;
}

int gorc::game::world::camera::camera_presenter::get_primary_focus(int camera_id) {
	return model->cameras[camera_id].focus;
}

void gorc::game::world::camera::camera_presenter::set_camera_focus(int camera_id, int focus_object_id) {
	model->cameras[camera_id].focus = focus_object_id;
}

void gorc::game::world::camera::camera_presenter::set_camera_state_flags(int flags) {
	model->state_flags = flag_set<flags::camera_state_flag>(flags);
}

void gorc::game::world::camera::camera_presenter::set_current_camera(int camera_id) {
	model->current_camera = camera_id;
}

void gorc::game::world::camera::camera_presenter::set_pov_shake(const vector<3>& pos_offset, const vector<3>& ang_offset, float pos_reset_speed, float ang_reset_speed) {
	auto& internal_cam = model->cameras[0];
	internal_cam.pos_offset = pos_offset;
	internal_cam.angle_offset = ang_offset;
	internal_cam.pos_reset_speed = pos_reset_speed;
	internal_cam.angle_reset_speed = ang_reset_speed;
}

void gorc::game::world::camera::camera_presenter::jk_set_pov_model(int player, int model_id) {
	if(model_id >= 0) {
		model->pov_model = &presenter.contentmanager->get_asset<content::assets::model>(model_id);
	}
	else {
		model->pov_model = nullptr;
	}

	presenter.key_presenter.stop_all_mix_keys(model->pov_key_mix_id);
}

void gorc::game::world::camera::camera_presenter::jk_set_waggle(int player, const vector<3>& move_vec, float speed) {
	model->waggle = move_vec;
	model->waggle_speed = speed;
}

int gorc::game::world::camera::camera_presenter::jk_play_pov_key(int player, int key, int priority, flag_set<flags::key_flag> flags) {
	return presenter.key_presenter.play_mix_key(model->pov_key_mix_id, key, priority, flags);
}

void gorc::game::world::camera::camera_presenter::jk_stop_pov_key(int player, int key_id, float delay) {
	presenter.key_presenter.stop_key(-1, key_id, delay);
}

void gorc::game::world::camera::camera_presenter::register_verbs(cog::verbs::verb_table& verbTable, application& components) {
	verbTable.add_verb<void, 0>("cyclecamera", [&components]() {
		components.current_level_presenter->camera_presenter.cycle_camera();
	});

	verbTable.add_verb<int, 0>("getcamerastateflags", [&components]() {
		return components.current_level_presenter->camera_presenter.get_camera_state_flags();
	});

	verbTable.add_verb<int, 0>("getcurrentcamera", [&components]() {
		return components.current_level_presenter->camera_presenter.get_current_camera();
	});

	verbTable.add_verb<int, 1>("getprimaryfocus", [&components](int camera_id) {
		return components.current_level_presenter->camera_presenter.get_primary_focus(camera_id);
	});

	verbTable.add_verb<void, 2>("setcamerafocus", [&components](int camera_id, int focus_thing_id) {
		components.current_level_presenter->camera_presenter.set_camera_focus(camera_id, focus_thing_id);
	});

	verbTable.add_verb<void, 1>("setcamerastateflags", [&components](int flags) {
		components.current_level_presenter->camera_presenter.set_camera_state_flags(flags);
	});

	verbTable.add_verb<void, 1>("setcurrentcamera", [&components](int camera_id) {
		components.current_level_presenter->camera_presenter.set_current_camera(camera_id);
	});

	verbTable.add_verb<void, 4>("setpovshake", [&components](vector<3> pos_offset, vector<3> ang_offset, float pos_reset_speed, float ang_reset_speed) {
		components.current_level_presenter->camera_presenter.set_pov_shake(pos_offset, ang_offset, pos_reset_speed, ang_reset_speed);
	});

	verbTable.add_verb<void, 2>("jksetpovmodel", [&components](int player, int model_id) {
		components.current_level_presenter->camera_presenter.jk_set_pov_model(player, model_id);
	});

	verbTable.add_verb<void, 3>("jksetwaggle", [&components](int player, vector<3> move_vec, float speed) {
		components.current_level_presenter->camera_presenter.jk_set_waggle(player, move_vec, speed);
	});

	verbTable.add_verb<int, 4>("jkplaypovkey", [&components](int player, int key, int priority, int key_flags) {
		return components.current_level_presenter->camera_presenter.jk_play_pov_key(player, key, priority, flag_set<flags::key_flag>(key_flags));
	});

	verbTable.add_verb<void, 3>("jkstoppovkey", [&components](int player, int key, float delay) {
		components.current_level_presenter->camera_presenter.jk_stop_pov_key(player, key, delay);
	});
}
