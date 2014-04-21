#include "actor_controller.h"
#include "game/world/level_presenter.h"
#include "game/world/level_model.h"
#include "game/constants.h"

void gorc::game::world::gameplay::actor_controller::update(int thing_id, double dt) {

    auto& thing = presenter.model->things[thing_id];

    if(thing.ai_mode_flags & flags::ai_mode_flag::turning_to_face_target) {
        // Get plane angle.
        auto lv = thing.orient.transform(make_vector(0.0f, 1.0f, 0.0f));
        auto plane_look = normalize(make_vector(get<0>(lv), get<1>(lv)));

        auto v = thing.ai_look_target;
        auto plane_v = normalize(make_vector(get<0>(v), get<1>(v)));

        float base_yaw = to_degrees(atan2f(get<1>(plane_look), get<0>(plane_look)));
        float target_yaw = to_degrees(atan2f(get<1>(plane_v), get<0>(plane_v)));

        if(fabs(base_yaw - target_yaw) <= 0.5) {
            thing.ai_mode_flags -= flags::ai_mode_flag::turning_to_face_target;
        }
        else {
            thing.orient = slerp(thing.orient, make_rotation<float>(make_vector(0.0f, 0.0f, 1.0f), target_yaw - 90.0f), thing.ai_move_speed * static_cast<float>(dt) * 1.0f);
        }
    }

    if(thing.ai_mode_flags & flags::ai_mode_flag::moving_toward_destination) {
        auto v = thing.ai_move_target - thing.position;
        get<2>(v) = 0.0f;

        float vlen = length(v);
        if(vlen > 0.005f) {
            thing.thrust = (v / vlen) * thing.ai_move_speed * static_cast<float>(dt) * 6.0f;
        }
        else {
            thing.thrust = make_zero_vector<3, float>();
            thing.ai_mode_flags -= flags::ai_mode_flag::moving_toward_destination;
        }
    }

    character_controller::update(thing_id, dt);
}
