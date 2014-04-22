#include "static_text.h"

gorc::gui::widgets::static_text::static_text(gui_view&, const content::assets::font& font, const std::string& text)
    : font(font), text(text) {
    return;
}

gorc::box<2, int> gorc::gui::widgets::static_text::get_minimum_size(const gui_view&) const {
    auto text_size = font.measure_text(text.begin(), text.end());
    return make_box(make_vector(0, 0),
            make_vector(get_size<0>(text_size) + margin.left + margin.right,
                    get_size<1>(text_size) + margin.top + margin.bottom));
}

void gorc::gui::widgets::static_text::draw(const time&, graphics::gui_renderer& renderer, int depth) const {
    int current_left = std::get<0>(get_range<0>(position));
    int current_top = std::get<0>(get_range<1>(position));

    char prev_c = 0;
    for(auto c : text) {
        if(c == '\n') {
            prev_c = 0;
            current_left = std::get<0>(get_range<0>(position));
            current_top += font.line_height();
        }
        else {
            current_left += font.get_kerning(prev_c, c);
            prev_c = c;

            const auto& glyph = font.get_glyph(c);
            renderer.draw_sprite(font.get_texture(), glyph.size + make_vector(current_left, current_top), glyph.tex_coords, depth);

            current_left += get_size<0>(glyph.size);
        }
    }
}
