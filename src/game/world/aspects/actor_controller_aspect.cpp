#include "actor_controller_aspect.h"
#include "game/world/level_model.h"
#include "game/world/events/thing_created.h"

gorc::game::world::aspects::actor_controller_aspect::actor_controller_aspect(component_system& cs)
    : inner_join_aspect(cs) {

    cs.bus.add_handler<events::thing_created>([&](events::thing_created const &e) {
        if(e.tpl.type == flags::thing_type::Actor) {
            cs.emplace_component<components::actor>(e.thing);
        }
    });
}

void gorc::game::world::aspects::actor_controller_aspect::update(time t, entity_id, components::actor&, components::thing& thing) {

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
            thing.orient = slerp(thing.orient, make_rotation<float>(make_vector(0.0f, 0.0f, 1.0f), target_yaw - 90.0f), thing.ai_move_speed * static_cast<float>(t.elapsed_as_seconds()) * 1.0f);
        }
    }

    if(thing.ai_mode_flags & flags::ai_mode_flag::moving_toward_destination) {
        auto v = thing.ai_move_target - thing.position;
        get<2>(v) = 0.0f;

        float vlen = length(v);
        if(vlen > 0.005f) {
            thing.thrust = (v / vlen) * thing.ai_move_speed * static_cast<float>(t.elapsed_as_seconds()) * 6.0f;
        }
        else {
            thing.thrust = make_zero_vector<3, float>();
            thing.ai_mode_flags -= flags::ai_mode_flag::moving_toward_destination;
        }
    }
    return;
}
