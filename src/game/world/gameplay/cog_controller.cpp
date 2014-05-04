#include "cog_controller.h"
#include "game/world/level_presenter.h"
#include "game/world/components/thing.h"
#include "game/world/level_model.h"
#include "game/constants.h"
#include "base/math/util.h"
#include <iostream>

using namespace gorc::math;

void gorc::game::world::gameplay::cog_controller::update(int thing_id, double dt) {
    thing_controller::update(thing_id, dt);

    // update thing attachment motion TODO
//    if(thing.attach_flags & flags::attach_flag::AttachedToThing) {
//        class thing& attached_thing = presenter.model->things[thing.attached_thing];
//        thing.position = attached_thing.position - thing.prev_attached_thing_position + thing.position;
//        thing.prev_attached_thing_position = attached_thing.position;
//    }
}
