#include "game/level_state.hpp"
#include "camera_presenter.hpp"
#include "game/world/level_presenter.hpp"
#include "game/world/keys/key_presenter.hpp"
#include "game/world/physics/physics_presenter.hpp"
#include "math/util.hpp"
#include "camera_model.hpp"
#include "game/world/level_model.hpp"
#include "game/world/physics/query.hpp"
#include "game/world/keys/components/pov_key_mix.hpp"
#include "game/world/components/pov_model.hpp"

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
}

void gorc::game::world::camera::camera_presenter::update(const gorc::time& time) {
    auto& selected_camera = model->cameras[model->current_camera];
    auto& cam = model->current_computed_state;

    const auto& focus_thing = levelmodel->get_thing(selected_camera.focus);
    const auto focus_thing_orient = focus_thing.orient
            * make_euler(selected_camera.angle_offset)
            * make_rotation(make_vector(1.0f, 0.0f, 0.0f), focus_thing.head_pitch);

    auto true_offset = focus_thing_orient.transform(focus_thing.eye_offset + selected_camera.base_offset + selected_camera.pos_offset);

    // Cast offset vector into world to detect collisions.
    maybe<physics::contact> contact;
    contact = presenter.physics_presenter->thing_segment_query(selected_camera.focus, true_offset,
            [&](thing_id tid) {
                auto t = presenter.model->get_thing(tid).type;
                return t == flags::thing_type::cog;
            },
            [&](surface_id surf_id) {
                return presenter.physics_presenter->surface_needs_collision_response(selected_camera.focus, surf_id);
            },
            contact);

    auto true_desired_position = focus_thing.position + true_offset;
    maybe_if(contact, [&](physics::contact const &cnt) {
        // Bring back by some small radius - 0.02?
        true_desired_position = cnt.position + normalize(focus_thing.position - cnt.position) * 0.02f;
    });

    // Calculate sector containing camera
    physics::segment_adjoin_path(physics::segment(focus_thing.position, true_desired_position), *levelmodel,
            at_id(levelmodel->sectors, focus_thing.sector), update_path_sector_scratch);
    cam.containing_sector = std::get<0>(update_path_sector_scratch.back());
    cam.position = true_desired_position;

    // Jiggle look and up by pov shake.
    cam.look = focus_thing_orient.transform(make_vector(0.0f, 1.0f, 0.0f));
    cam.up = focus_thing_orient.transform(make_vector(0.0f, 0.0f, 1.0f));

    if(selected_camera.draw_focus) {
        cam.focus_not_drawn_thing = invalid_id;
    }
    else {
        cam.focus_not_drawn_thing = selected_camera.focus;
    }

    auto &pov_model = levelmodel->ecs.get_unique_component<components::pov_model>(
            selected_camera.focus);

    if(selected_camera.draw_pov_model) {
        cam.pov_model = pov_model.model;
    }
    else {
        cam.pov_model = nothing;
    }

    // Calculate pov model waggle.
    const auto& player_thing = levelmodel->get_thing(presenter.get_local_player_thing());
    auto player_speed = length(make_vector(get<0>(player_thing.vel), get<1>(player_thing.vel))) / player_thing.max_vel;
    float actual_waggle_speed = player_speed * pov_model.waggle_speed * static_cast<float>(time.elapsed_as_seconds()) * (1.0f / 60.0f);
    pov_model.waggle_time += actual_waggle_speed;
    float waggle_up = -cosf(pov_model.waggle_time * 2.0f);
    float waggle_left = sinf(pov_model.waggle_time);
    auto new_offset = player_speed *
                      make_vector(waggle_up * get<0>(pov_model.waggle) * (9.0f / 16.0f),
                                  waggle_left * get<1>(pov_model.waggle) * (16.0f / 9.0f),
                                  waggle_up * get<2>(pov_model.waggle));
    cam.pov_model_offset = lerp(cam.pov_model_offset, new_offset, 0.1f);

    for(auto& camera_state : model->cameras) {
        camera_state.angle_offset = camera_state.angle_offset - clamp_length(camera_state.angle_offset, 0.0f, camera_state.angle_reset_speed * static_cast<float>(time.elapsed_as_seconds()));
        camera_state.pos_offset = camera_state.pos_offset - clamp_length(camera_state.pos_offset, 0.0f, camera_state.pos_reset_speed * static_cast<float>(time.elapsed_as_seconds()));
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

gorc::thing_id gorc::game::world::camera::camera_presenter::get_primary_focus(int camera_id) {
    return model->cameras[camera_id].focus;
}

void gorc::game::world::camera::camera_presenter::set_camera_focus(int camera_id, thing_id focus_object_id) {
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

void gorc::game::world::camera::camera_presenter::jk_set_pov_model(thing_id tid, model_id mid) {
    auto &pov_model = levelmodel->ecs.get_unique_component<components::pov_model>(tid);

    if(!mid.is_valid()) {
        pov_model.model = nothing;
    }
    else {
        pov_model.model = get_asset(*presenter.contentmanager, mid);
    }

    presenter.key_presenter->stop_all_mix_keys(tid, true);
}

void gorc::game::world::camera::camera_presenter::jk_set_waggle(thing_id tid, const vector<3>& move_vec, float speed) {
    auto &pov_model = levelmodel->ecs.get_unique_component<components::pov_model>(tid);
    pov_model.waggle = move_vec;
    pov_model.waggle_speed = speed;
}

gorc::thing_id gorc::game::world::camera::camera_presenter::jk_play_pov_key(thing_id tid, keyframe_id key, int priority, flag_set<flags::key_flag> flags) {
    return presenter.key_presenter->play_mix_key(tid, true, key, priority, flags);
}

void gorc::game::world::camera::camera_presenter::jk_stop_pov_key(thing_id, thing_id key_id, float delay) {
    presenter.key_presenter->stop_key(invalid_id, key_id, delay);
}

void gorc::game::world::camera::camera_presenter::register_verbs(cog::verb_table &verbs, level_state &components) {
    verbs.add_verb("cyclecamera", [&components]() {
        components.current_level_presenter->camera_presenter->cycle_camera();
    });

    verbs.add_verb("getcamerastateflags", [&components]() {
        return components.current_level_presenter->camera_presenter->get_camera_state_flags();
    });

    verbs.add_verb("getcurrentcamera", [&components]() {
        return components.current_level_presenter->camera_presenter->get_current_camera();
    });

    verbs.add_verb("getprimaryfocus", [&components](int camera_id) {
        return components.current_level_presenter->camera_presenter->get_primary_focus(camera_id);
    });

    verbs.add_safe_verb("setcamerafocus", cog::value(), [&components](int camera_id, thing_id focus_thing_id) {
        components.current_level_presenter->camera_presenter->set_camera_focus(camera_id, focus_thing_id);
    });

    verbs.add_verb("setcamerastateflags", [&components](int flags) {
        components.current_level_presenter->camera_presenter->set_camera_state_flags(flags);
    });

    verbs.add_verb("setcurrentcamera", [&components](int camera_id) {
        components.current_level_presenter->camera_presenter->set_current_camera(camera_id);
    });

    verbs.add_verb("setpovshake", [&components](vector<3> pos_offset, vector<3> ang_offset, float pos_reset_speed, float ang_reset_speed) {
        components.current_level_presenter->camera_presenter->set_pov_shake(pos_offset, ang_offset, pos_reset_speed, ang_reset_speed);
    });

    verbs.add_safe_verb("jksetpovmodel", cog::value(), [&components](thing_id player, model_id mid) {
        components.current_level_presenter->camera_presenter->jk_set_pov_model(player, mid);
    });

    verbs.add_safe_verb("jksetwaggle", cog::value(), [&components](thing_id player, vector<3> move_vec, float speed) {
        components.current_level_presenter->camera_presenter->jk_set_waggle(player, move_vec, speed);
    });

    verbs.add_safe_verb("jkplaypovkey", thing_id(invalid_id), [&components](thing_id player, keyframe_id key, int priority, int key_flags) {
        return components.current_level_presenter->camera_presenter->jk_play_pov_key(player, key, priority, flag_set<flags::key_flag>(key_flags));
    });

    verbs.add_safe_verb("jkstoppovkey", cog::value(), [&components](thing_id player, thing_id key, float delay) {
        components.current_level_presenter->camera_presenter->jk_stop_pov_key(player, key, delay);
    });
}
