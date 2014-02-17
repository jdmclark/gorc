#pragma once

#include "framework/view.h"
#include "framework/input/gui_input_adapter.h"
#include "framework/gui/widget.h"
#include "framework/utility/make_unique.h"
#include "framework/graphics/gui_renderer.h"
#include "framework/utility/pair_range.h"
#include <unordered_map>
#include <memory>

namespace gorc {
namespace gui {

class gui_view : public gorc::view {
private:
    input::gui_input_adapter<gui_view> input_adapter;
    std::unordered_multimap<widget const*, std::unique_ptr<widget>> parent_widget_map;
    widget& root_widget;
    graphics::gui_renderer gui_renderer;

    int mouse_over_widget_depth = 0;
    maybe<widget*> mouse_over_widget;
    maybe<widget*> keyboard_focus_widget;

    void resize_widget(const box<2, int>& position, widget& w);
    void draw_widget(const time& time, const widget& w, int depth);
    std::tuple<int, maybe<widget*>> get_mouse_over_widget(const vector<2, int>& mouse_position, widget& w, int depth);

public:
    explicit gui_view(const content::assets::shader& gui_shader);

    void layout();
    virtual void resize(const box<2, int>& view_size) override;
    virtual void draw(const time& time, const box<2, int>& view_size, graphics::render_target&) override;
    virtual maybe<input::input_adapter*> get_input_adapter() override;

    inline const widget& get_root() const {
        return root_widget;
    }

    inline widget& get_root() {
        return root_widget;
    }

    inline auto get_children(const widget& parent) -> decltype(make_pair_range(parent_widget_map.equal_range(&parent))) {
        return make_pair_range(parent_widget_map.equal_range(&parent));
    }

    inline auto get_children(const widget& parent) const -> decltype(make_pair_range(parent_widget_map.equal_range(&parent))) {
        return make_pair_range(parent_widget_map.equal_range(&parent));
    }

    template <typename T, typename... ArgT> inline T& add_child(const widget& parent, ArgT&&... args) {
        return *reinterpret_cast<T*>(parent_widget_map.emplace(&parent, make_unique<T>(*this, std::forward<ArgT>(args)...))->second.get());
    }

    void set_mouse_cursor_position(const time& time, const vector<2, int>& cursor_pos);
    bool wants_mouse_focus() const;
    bool wants_keyboard_focus() const;

    void on_mouse_button_up(const time& time, const vector<2, int>& position, sf::Mouse::Button b);
    void on_mouse_button_down(const time& time, const vector<2, int>& position, sf::Mouse::Button b);
    void on_keyboard_key_up(const time& time, sf::Keyboard::Key k, bool shift_down, bool ctrl_down, bool alt_down);
    void on_keyboard_key_down(const time& time, sf::Keyboard::Key k, bool shift_down, bool ctrl_down, bool alt_down);
    void on_keyboard_text_entered(const time& time, char ch);
};

}
}
