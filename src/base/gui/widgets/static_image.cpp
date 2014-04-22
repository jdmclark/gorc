#include "static_image.h"

gorc::gui::widgets::static_image::static_image(gui_view&, const content::assets::texture& sprite_sheet, const box<2, int>& sz)
    : sprite_sheet(sprite_sheet), graphics_size(sz) {
    return;
}

gorc::box<2, int> gorc::gui::widgets::static_image::get_minimum_size(const gui_view&) const {
    return graphics_size;
}

void gorc::gui::widgets::static_image::draw(const time&, graphics::gui_renderer& renderer, int depth) const {
    renderer.draw_sprite(sprite_sheet, position, graphics_size, depth);
}
