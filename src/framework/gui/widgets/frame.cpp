#include "frame.h"

gorc::gui::widgets::frame::frame(gui_view&, const content::assets::texture& sprite_sheet) : sprite_sheet(sprite_sheet) {
    return;
}

void gorc::gui::widgets::frame::draw(const time& time, graphics::gui_renderer& renderer, int depth) const {
    auto part_offset = make_vector(0, 8 * static_cast<int>(style));
    renderer.draw_frame(sprite_sheet, position, part_offset, depth);
}
