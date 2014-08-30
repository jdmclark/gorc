#pragma once

#include "base/gui/gui_view.h"
#include "base/content/content_manager.h"

namespace gorc {
namespace client {

class hud_view : public gorc::gui::gui_view {
public:
    hud_view(content::content_manager& manager, std::string enginename);
};

}
}
