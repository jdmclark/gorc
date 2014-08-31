#include <iomanip>
#include <sstream>

#include "hud_view.h"
#include "client/application.h"
#include "content/assets/bitmap.h"
#include "content/assets/colormap.h"
#include "content/assets/sfont.h"
#include "base/math/util.h"
#include "base/gui/gui_view.h"
#include "base/content/assets/shader.h"
#include "base/gui/widgets/static_image.h"
#include "base/gui/widgets/static_text.h"

gorc::client::hud_view::hud_view(content::content_manager& manager)
    : gui_view(manager.load<content::assets::shader>("gui.glsl")),
    dfltcmp(manager.load<content::assets::colormap>("dflt.cmp")),
    msgfont(manager.load<content::assets::sfont>("msgfont16.sft", dfltcmp)),
    healthfont(manager.load<content::assets::sfont>("helthnum16.sft", dfltcmp)),
    shieldfont(manager.load<content::assets::sfont>("armornum16.sft", dfltcmp)),
    ammofont(manager.load<content::assets::sfont>("amonums16.sft", dfltcmp)),
    statusleft(manager.load<content::assets::bitmap>("statusleft16.bm", dfltcmp)),
    statusright(manager.load<content::assets::bitmap>("statusright16.bm", dfltcmp)),
    health(manager.load<content::assets::bitmap>("sthealth16.bm", dfltcmp)),
    shields(manager.load<content::assets::bitmap>("stshield16.bm", dfltcmp)),
    force(manager.load<content::assets::bitmap>("stfrc16.bm", dfltcmp)),
    force_super(manager.load<content::assets::bitmap>("stfrcsuper16.bm", dfltcmp)), //TODO: Super Force
    fieldlight(manager.load<content::assets::bitmap>("stfieldlite16.bm", dfltcmp)),
    battery(manager.load<content::assets::bitmap>("stbat16.bm", dfltcmp)),

    left(add_child<gui::widgets::static_image>(get_root(), statusleft.cels[0].color, make_box(make_vector(0, 0), make_vector(59, 60)))),
    left_mask(add_child<gui::widgets::static_image>(get_root(), health.cels[7].color, make_box(make_vector(0, 0), make_vector(24, 15)))),
    left_health(add_child<gui::widgets::dynamic_image>(get_root(), health, make_box(make_vector(0, 0), make_vector(18, 18)))),
    left_shields(add_child<gui::widgets::dynamic_image>(get_root(), shields, make_box(make_vector(0, 0), make_vector(38, 38)))),
    right(add_child<gui::widgets::static_image>(get_root(), statusright.cels[0].color, make_box(make_vector(0, 0), make_vector(59, 60)))),
    right_mask(add_child<gui::widgets::static_image>(get_root(), health.cels[7].color, make_box(make_vector(0, 0), make_vector(35, 11)))),
    right_force(add_child<gui::widgets::dynamic_image>(get_root(), force, make_box(make_vector(0, 0), make_vector(43, 22)))), //TODO: Check force level
    right_fieldlight(add_child<gui::widgets::static_image>(get_root(), fieldlight.cels[1].color, make_box(make_vector(0, 0), make_vector(10, 11)))),
    right_battery(add_child<gui::widgets::dynamic_image>(get_root(), battery, make_box(make_vector(0, 0), make_vector(27, 14)))),
    health_amount(add_child<gui::widgets::static_text>(get_root(), healthfont, "999")),
    shields_amount(add_child<gui::widgets::static_text>(get_root(), shieldfont, "999")),
    ammo_amount(add_child<gui::widgets::static_text>(get_root(), ammofont, "000")) {

    left.horizontal_align = gui::layout::horizontal_align_style::left;
    left.vertical_align = gui::layout::vertical_align_style::bottom;

    //HACK: Force a black box around the '000' text so we can draw our own stuff
    left_mask.horizontal_align = gui::layout::horizontal_align_style::left;
    left_mask.vertical_align = gui::layout::vertical_align_style::bottom;
    left_mask.padding.bottom = 10;
    left_mask.padding.left = 13;
    left_mask.zbias = 1;

    right.horizontal_align = gui::layout::horizontal_align_style::right;
    right.vertical_align = gui::layout::vertical_align_style::bottom;

    //HACK: Force a black box around the '000' text for ammo
    right_mask.horizontal_align = gui::layout::horizontal_align_style::right;
    right_mask.vertical_align = gui::layout::vertical_align_style::bottom;
    right_mask.padding.right = 11;
    right_mask.padding.bottom = 30;
    right_mask.zbias = 1;

    left_health.horizontal_align = gui::layout::horizontal_align_style::left;
    left_health.vertical_align = gui::layout::vertical_align_style::bottom;
    left_health.padding.left = 21;
    left_health.padding.bottom = 21;
    left_health.zbias = 2;

    left_shields.horizontal_align = gui::layout::horizontal_align_style::left;
    left_shields.vertical_align = gui::layout::vertical_align_style::bottom;
    left_shields.padding.left = 12;
    left_shields.padding.bottom = 12;

    right_force.horizontal_align = gui::layout::horizontal_align_style::right;
    right_force.vertical_align = gui::layout::vertical_align_style::bottom;
    right_force.padding.right = 7;
    right_force.padding.bottom = 7;
    right_force.set_cel(15);

    right_fieldlight.horizontal_align = gui::layout::horizontal_align_style::right;
    right_fieldlight.vertical_align = gui::layout::vertical_align_style::bottom;
    right_fieldlight.padding.right = 49;
    right_fieldlight.padding.bottom = 40;
    right_fieldlight.visible = false;

    right_battery.horizontal_align = gui::layout::horizontal_align_style::right;
    right_battery.vertical_align = gui::layout::vertical_align_style::bottom;
    right_battery.padding.right = 22;
    right_battery.padding.bottom = 46;

    health_amount.horizontal_align = gui::layout::horizontal_align_style::left;
    health_amount.vertical_align = gui::layout::vertical_align_style::bottom;
    health_amount.padding.bottom = 18;
    health_amount.padding.left = 13;
    health_amount.zbias = 2;

    shields_amount.horizontal_align = gui::layout::horizontal_align_style::left;
    shields_amount.vertical_align = gui::layout::vertical_align_style::bottom;
    shields_amount.padding.bottom = 10;
    shields_amount.padding.left = 23;
    shields_amount.zbias = 2;

    ammo_amount.horizontal_align = gui::layout::horizontal_align_style::right;
    ammo_amount.vertical_align = gui::layout::vertical_align_style::bottom;
    ammo_amount.padding.right = 12;
    ammo_amount.padding.bottom = 30;
    ammo_amount.zbias = 2;

    auto& message_label = add_child<gui::widgets::static_text>(get_root(), msgfont, "GORC TEST");
    message_label.horizontal_align = gui::layout::horizontal_align_style::center;
    message_label.vertical_align = gui::layout::vertical_align_style::top;
}

void gorc::client::hud_view::update(int currentplayer, bool fieldlight, int batt_amount, int health, int max_health, int shields, int max_shields, int force, int ammo) {
    int health_frac = clamp(5 - (health / (max_health / 5)), 0, 5);
    int shield_frac = clamp(10 - (shields / (max_shields / 10)), 0, 10); //TODO: Pull proper max shields value
    int batt_frac = 200 / 6;
    int force_frac = clamp(15 - (force / (400 / 15)), 0, 15); //TODO: Pull proper max force value from getInv(20)*50, which is currently broken.

    std::ostringstream a;
    a << std::setw( 3 ) << std::setfill( '0' ) << ammo;
    std::string ammo_padded = (ammo > 0 ? a.str() : "---");

    std::ostringstream h;
    h << std::setw( 3 ) << std::setfill( '0' ) << health;
    std::string health_padded = h.str();

    std::ostringstream sh;
    sh << std::setw( 3 ) << std::setfill( '0' ) << shields;
    std::string shield_padded = sh.str();

    left_health.set_cel(health_frac);
    left_shields.set_cel(shield_frac);
    right_battery.set_cel(batt_amount / batt_frac);
    right_fieldlight.visible = fieldlight;
    right_force.set_cel(force_frac);

    health_amount.text = health_padded;
    shields_amount.text = shield_padded;
    ammo_amount.text = ammo_padded;
}
