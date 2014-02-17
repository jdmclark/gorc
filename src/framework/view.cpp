#include "view.h"

gorc::view::~view() {
    return;
}

void gorc::view::resize(const box<2, int>& view_size) {
    return;
}

void gorc::view::draw(const time& time, const box<2, int>& view_size, graphics::render_target&) {
    return;
}

gorc::maybe<gorc::input::input_adapter*> gorc::view::get_input_adapter() {
    return gorc::maybe<gorc::input::input_adapter*>();
}
