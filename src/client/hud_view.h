#pragma once

#include "framework/gui/gui_view.h"
#include "framework/content/manager.h"

namespace gorc {
namespace client {

class hud_view : public gorc::gui::gui_view {
public:
	hud_view(content::manager& manager);
};

}
}
