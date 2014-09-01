#pragma once

#include <memory>

#include "client/application.h"
#include "base/gui/gui_view.h"
#include "base/gui/widgets/static_image.h"
#include "base/gui/widgets/dynamic_image.h"
#include "base/gui/widgets/static_text.h"
#include "base/content/content_manager.h"
#include "content/assets/bitmap.h"
#include "content/assets/colormap.h"
#include "content/assets/sfont.h"
#include "game/world/level_model.h"
#include "game/world/level_presenter.h"
#include "game/world/inventory/inventory_presenter.h"

class level_presenter;

namespace gorc {
namespace client {

class hud_view : public gorc::gui::gui_view {
public:
    hud_view(content::content_manager& manager);
    virtual void update(int current_player, gorc::game::world::inventory::inventory_presenter* inventory, gorc::game::world::level_presenter* level);
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
    gui::widgets::dynamic_image& left_health;
    gui::widgets::dynamic_image& left_shields;
    gui::widgets::static_image& right;
    gui::widgets::dynamic_image& right_force;
    gui::widgets::static_image& right_fieldlight;
    gui::widgets::dynamic_image& right_battery;

    gui::widgets::static_text& health_amount;
    gui::widgets::static_text& shields_amount;
    gui::widgets::static_text& ammo_amount;

    const int weapon_ammo [11] { -1, -1, 11, 11, 4, 12, 12, 15, 8, 12, -1 };
};

}
}
