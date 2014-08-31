#pragma once

#include "client/application.h"
#include "base/gui/gui_view.h"
#include "base/gui/widgets/static_image.h"
#include "base/gui/widgets/dynamic_image.h"
#include "base/gui/widgets/static_text.h"
#include "base/content/content_manager.h"
#include "content/assets/bitmap.h"
#include "content/assets/colormap.h"
#include "content/assets/sfont.h"


namespace gorc {
namespace client {

class hud_view : public gorc::gui::gui_view {
public:
    hud_view(content::content_manager& manager);
    virtual void update(int currentplayer, bool fieldlight, int batt_amount, int health, int max_health, int shields, int max_shields, int force, int ammo);
private:
    const content::assets::colormap& dfltcmp;
    const content::assets::sfont& msgfont;
    const content::assets::sfont& healthfont;
    const content::assets::sfont& shieldfont;
    const content::assets::sfont& ammofont;
    const content::assets::bitmap& statusleft;
    const content::assets::bitmap& statusright;
    const content::assets::bitmap& health;
    const content::assets::bitmap& shields;
    const content::assets::bitmap& force;
    const content::assets::bitmap& force_super;
    const content::assets::bitmap& fieldlight;
    const content::assets::bitmap& battery;

    gui::widgets::static_image& left;
    gui::widgets::static_image& left_mask;
    gui::widgets::dynamic_image& left_health;
    gui::widgets::dynamic_image& left_shields;
    gui::widgets::static_image& right;
    gui::widgets::static_image& right_mask;
    gui::widgets::dynamic_image& right_force;
    gui::widgets::static_image& right_fieldlight;
    gui::widgets::dynamic_image& right_battery;

    gui::widgets::static_text& health_amount;
    gui::widgets::static_text& shields_amount;
    gui::widgets::static_text& ammo_amount;

    int incr = 0;
};

}
}
