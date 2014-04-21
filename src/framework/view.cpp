#include "view.h"

gorc::view::~view() {
    return;
}

void gorc::view::resize(const box<2, int>&) {
    return;
}

void gorc::view::draw(const time&, const box<2, int>&, graphics::render_target&) {
    return;
}

gorc::maybe<gorc::input::input_adapter*> gorc::view::get_input_adapter() {
    return nothing;
}
