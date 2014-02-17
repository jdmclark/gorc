#include "wrapped_text.h"

gorc::gui::widgets::wrapped_text::wrapped_text(gui_view&, const content::assets::font& font, const std::string& text)
    : font(font), text(text) {
    return;
}

gorc::box<2, int> gorc::gui::widgets::wrapped_text::get_minimum_size(const gui_view&) const {
    return make_box(make_vector(0, 0), make_vector(0, 0));
}

void gorc::gui::widgets::wrapped_text::draw_range(std::string::const_iterator begin, std::string::const_iterator end,
        int left, int top, graphics::gui_renderer& r, int depth) const {
    int current_left = std::get<0>(get_range<0>(position));

    char prev_c = 0;
    for(auto it = begin; it != end; ++it) {
        char c = *it;
        current_left += font.get_kerning(prev_c, c);
        prev_c = c;

        const auto& glyph = font.get_glyph(c);
        r.draw_sprite(font.get_texture(), glyph.size + make_vector(current_left, top), glyph.tex_coords, depth);

        current_left += get_size<0>(glyph.size);
    }
}

void gorc::gui::widgets::wrapped_text::draw(const time& time, graphics::gui_renderer& renderer, int depth) const {
    int current_top = std::get<0>(get_range<1>(position));
    int right_bound = std::get<1>(get_range<0>(position));

    auto line_begin = text.begin();
    while(line_begin != text.end()) {

        auto split_position = line_begin;

        int current_left = std::get<0>(get_range<0>(position));
        char prev_c = 0;
        for(auto it = line_begin; it != text.end(); ++it) {
            if(split_position != line_begin && current_left > right_bound) {
                break;
            }
            else if(*it == '\n') {
                split_position = it;
                break;
            }
            else {
                char c = *it;
                if(c == ' ') {
                    split_position = it;
                }

                current_left += font.get_kerning(prev_c, c);
                prev_c = c;

                const auto& glyph = font.get_glyph(c);
                current_left += get_size<0>(glyph.size);
            }
        }

        if(line_begin == split_position) {
            // Reached the end.
            draw_range(line_begin, text.end(), std::get<0>(get_range<0>(position)), current_top, renderer, depth);
            break;
        }
        else {
            draw_range(line_begin, split_position, std::get<0>(get_range<0>(position)), current_top, renderer, depth);
            current_top += font.line_height();
            line_begin = split_position + 1;
        }
    }
}
