#include "grid_panel.h"
#include "framework/gui/gui_view.h"

gorc::gui::widgets::grid_panel::grid_panel(gui_view& v, const content::assets::texture& tex,
        int col_width, int n_cols, int row_width, int n_rows, int gutter_size)
    : tex(tex), row_width(row_width), n_rows(n_rows), col_width(col_width), n_cols(n_cols), gutter_size(gutter_size) {
    return;
}

gorc::box<2, int> gorc::gui::widgets::grid_panel::get_child_position(const gui_view& v, const widget& child) const {
    int left, top;
    std::tie(left, std::ignore) = get_range<0>(position);
    std::tie(top, std::ignore) = get_range<1>(position);

    int x_off = col_width + gutter_size;
    int y_off = row_width + gutter_size;

    int x_pos = left + x_off * get<0>(child.grid_position);
    int y_pos = top + y_off * get<1>(child.grid_position);

    int x1_pos = x_pos + x_off * (child.column_span - 1) + col_width;
    int y1_pos = y_pos + y_off * (child.row_span - 1) + row_width;

    return get_default_child_position(v, child, make_box(make_vector(x_pos, y_pos), make_vector(x1_pos, y1_pos)));
}

gorc::box<2, int> gorc::gui::widgets::grid_panel::get_minimum_size(const gui_view& v) const {
    return make_box(make_vector(0, 0), make_vector(
            (n_cols - 1) * (col_width + gutter_size) + col_width,
            (n_rows - 1) * (row_width + gutter_size) + row_width));
}

void gorc::gui::widgets::grid_panel::draw(const time& time, graphics::gui_renderer& r, int depth) const {
    if(draw_wireframe) {
        constexpr auto frame_texcoords = make_vector(0, 32);

        int top, left;
        std::tie(left, std::ignore) = get_range<0>(position);
        std::tie(top, std::ignore) = get_range<1>(position);

        const int left_offset = col_width + gutter_size;
        const int top_offset = row_width + gutter_size;

        for(int i = 0; i < n_cols; ++i) {
            for(int j = 0; j < n_rows; ++j) {
                const int x_pos = left + left_offset * i;
                const int y_pos = top + top_offset * j;
                r.draw_frame(tex, make_box(make_vector(x_pos, y_pos), make_vector(x_pos + col_width, y_pos + row_width)),
                        frame_texcoords, depth);
            }
        }
    }
    return;
}
