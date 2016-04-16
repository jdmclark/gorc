#include "armed_mode_changed.hpp"

gorc::game::world::events::armed_mode_changed::armed_mode_changed(thing_id thing,
                                                                  flags::armed_mode mode)
    : thing(thing), mode(mode) {
    return;
}
