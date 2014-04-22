#include "hud_view.h"
#include "content/assets/bitmap.h"
#include "content/assets/colormap.h"
#include "content/assets/sfont.h"
#include "base/content/assets/shader.h"
#include "base/gui/widgets/static_image.h"
#include "base/gui/widgets/static_text.h"

gorc::client::hud_view::hud_view(content::content_manager& manager)
    : gui_view(manager.load<content::assets::shader>("gui.glsl")) {
    auto& dfltcmp = manager.load<content::assets::colormap>("dflt.cmp");
    const auto& statusleft = manager.load<content::assets::bitmap>("statusleft16.bm", dfltcmp);
    const auto& statusright = manager.load<content::assets::bitmap>("statusright16.bm", dfltcmp);

    auto& left = add_child<gui::widgets::static_image>(get_root(), statusleft.cels[0].color, make_box(make_vector(0, 0), make_vector(59, 60)));
    left.horizontal_align = gui::layout::horizontal_align_style::left;
    left.vertical_align = gui::layout::vertical_align_style::bottom;

    auto& right = add_child<gui::widgets::static_image>(get_root(), statusright.cels[0].color, make_box(make_vector(0, 0), make_vector(59, 60)));
    right.horizontal_align = gui::layout::horizontal_align_style::right;
    right.vertical_align = gui::layout::vertical_align_style::bottom;

    auto& msgfont = manager.load<content::assets::sfont>("msgfont16.sft", dfltcmp);

    auto& message_label = add_child<gui::widgets::static_text>(get_root(), msgfont, "GORC TEST");
    message_label.horizontal_align = gui::layout::horizontal_align_style::center;
    message_label.vertical_align = gui::layout::vertical_align_style::top;
}
