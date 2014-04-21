#include "edit_box.h"
#include "framework/gui/gui_view.h"

gorc::gui::widgets::edit_box::edit_box(gui_view& view, const content::assets::texture& skin,
        const content::assets::font& font, const std::string& text) : skin(skin), font(font), buffer(text) {
    container_box = &view.add_child<frame>(*this, skin);
    container_box->dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };
    container_box->padding = 2;
    container_box->style = widgets::frame_style::inset;
}

gorc::box<2, int> gorc::gui::widgets::edit_box::get_minimum_size(const gui_view&) const {
    return make_box(make_vector(0, 0),
            make_vector(margin.left + margin.right + padding.left + padding.right,
                    font.line_height() + margin.top + margin.bottom + padding.top + padding.bottom));
}

void gorc::gui::widgets::edit_box::set_scroll_position(size_t pos) {
    scroll_position = clamp(pos, 0UL, buffer.size());
}

void gorc::gui::widgets::edit_box::set_cursor_position(size_t pos, const time& time) {
    cursor_position = clamp(pos, 0UL, buffer.size());
    focus_time = time.get_time_stamp();

    // Determine if cursor position is still inside box.
    if(cursor_position < scroll_position) {
        set_scroll_position(cursor_position);
    }
    else {
        auto text_sz = font.measure_text(buffer.begin() + scroll_position, buffer.begin() + cursor_position);
        int sz_diff = get_size<0>(text_sz) - get_size<0>(container_box->position) + container_box->padding.left + container_box->padding.right;

        char prev_ch = 0;
        while(sz_diff > 0) {
            // Span between beginning of range and cursor is too large to fit in box.

            char ch = buffer[scroll_position];
            sz_diff -= font.get_kerning(prev_ch, ch);
            prev_ch = ch;
            sz_diff -= get_size<0>(font.get_glyph(ch).size);
            set_scroll_position(scroll_position + 1);
        }
    }
}

bool gorc::gui::widgets::edit_box::wants_mouse_input() const {
    return true;
}

void gorc::gui::widgets::edit_box::on_mouse_down(const time& time, const vector<2, int>& cursor_pos, sf::Mouse::Button btn) {
    if(btn == sf::Mouse::Left) {
        auto text_cursor_pos = cursor_pos - make_vector(padding.left, padding.top);

        size_t next_cursor_pos = buffer.size();

        int current_left = 0;
        char prev_c = 0;
        for(size_t i = scroll_position; i < buffer.size(); ++i) {
            char c = buffer[i];
            current_left += font.get_kerning(prev_c, c);
            prev_c = c;

            const auto& glyph = font.get_glyph(c);
            current_left += get_size<0>(glyph.size);

            if(current_left >= get<0>(text_cursor_pos)) {
                // User is clicking on current glyph.
                if(current_left - get_size<0>(glyph.size) / 2 >= get<0>(text_cursor_pos)) {
                    next_cursor_pos = i;
                }
                else {
                    next_cursor_pos = i + 1;
                }
                break;
            }
        }

        set_cursor_position(next_cursor_pos, time);
    }
}

bool gorc::gui::widgets::edit_box::wants_keyboard_focus() const {
    return true;
}

void gorc::gui::widgets::edit_box::on_gain_keyboard_focus(const time&) {
    has_focus = true;
}

void gorc::gui::widgets::edit_box::on_lost_keyboard_focus(const time&) {
    has_focus = false;
}

void gorc::gui::widgets::edit_box::on_text_entered(const time& time, char ch) {
    buffer.insert(buffer.begin() + clamp(cursor_position, 0UL, buffer.size()), ch);
    set_cursor_position(cursor_position + 1, time);
}

void gorc::gui::widgets::edit_box::on_key_down(const time& time, sf::Keyboard::Key k, bool, bool ctrl_down, bool alt_down) {
    if(ctrl_down || alt_down) {
        return;
    }

    switch(k) {
    case sf::Keyboard::Left:
        set_cursor_position(cursor_position - 1, time);
        break;

    case sf::Keyboard::Right:
        set_cursor_position(cursor_position + 1, time);
        break;

    case sf::Keyboard::End:
        set_cursor_position(buffer.size(), time);
        break;

    case sf::Keyboard::Home:
        set_cursor_position(0, time);
        break;

    case sf::Keyboard::Delete:
        if(cursor_position < buffer.size()) {
            buffer.erase(buffer.begin() + clamp(cursor_position, 0UL, buffer.size()));
        }
        break;

    case sf::Keyboard::BackSpace:
        if(cursor_position > 0) {
            set_cursor_position(cursor_position - 1, time);
            buffer.erase(buffer.begin() + clamp(cursor_position, 0UL, buffer.size()));
        }
        break;

    default:
        // Silently consume unhandled input.
        // Remaining input is handled by text input.
        break;
    }
}

void gorc::gui::widgets::edit_box::draw(const time& time, graphics::gui_renderer& renderer, int depth) const {
    double elapsed_time = time.get_time_stamp() - focus_time;
    bool inner_draw_cursor = has_focus && std::fmod(elapsed_time, 1.0) <= 0.5;

    int current_left = std::get<0>(get_range<0>(container_box->position)) + container_box->padding.left;
    int current_top = std::get<0>(get_range<1>(container_box->position)) + container_box->padding.top;
    int right = std::get<1>(get_range<0>(container_box->position)) - container_box->padding.right;

    char prev_c = 0;
    size_t spos = clamp(scroll_position, 0UL, buffer.size());
    for(size_t i = spos; i < buffer.size(); ++i) {

        if(cursor_position == i && inner_draw_cursor) {
            const auto& cur_glyph = font.get_glyph('|');
            renderer.draw_sprite(font.get_texture(), cur_glyph.size + make_vector(current_left, current_top) - make_vector(get_size<0>(cur_glyph.size) / 2, 0),
                    cur_glyph.tex_coords, depth + 3);
        }

        char c = buffer[i];
        current_left += font.get_kerning(prev_c, c);
        prev_c = c;


        const auto& glyph = font.get_glyph(c);

        int next_left = current_left + get_size<0>(glyph.size);
        if(next_left > right) {
            break;
        }

        renderer.draw_sprite(font.get_texture(), glyph.size + make_vector(current_left, current_top), glyph.tex_coords, depth + 2);

        current_left = next_left;
    }

    if(cursor_position == buffer.size() && inner_draw_cursor) {
        const auto& cur_glyph = font.get_glyph('|');
        renderer.draw_sprite(font.get_texture(), cur_glyph.size + make_vector(current_left, current_top) - make_vector(get_size<0>(cur_glyph.size) / 2, 0),
                cur_glyph.tex_coords, depth + 3);
    }

    return;
}
