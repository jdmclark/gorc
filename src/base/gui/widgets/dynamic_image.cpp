#include "dynamic_image.h"

gorc::gui::widgets::dynamic_image::dynamic_image(gui_view&, const content::assets::bitmap& sprite_sheet, const box<2, int>& sz)
    : sprite_sheet(sprite_sheet), graphics_size(sz) {
    return;
}

gorc::box<2, int> gorc::gui::widgets::dynamic_image::get_minimum_size(const gui_view&) const {
    return graphics_size;
}

void gorc::gui::widgets::dynamic_image::draw(const time&, graphics::gui_renderer& renderer, int depth) const {
    renderer.draw_sprite(sprite_sheet.cels[current_cel].color, position, graphics_size, depth);
}

void gorc::gui::widgets::dynamic_image::set_cel(int cel) {
    current_cel = cel;
}
