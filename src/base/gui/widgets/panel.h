#pragma once

#include "base/gui/widget.h"

namespace gorc {
namespace gui {

class gui_view;

namespace widgets {

class panel : public widget {
public:
    panel(gui_view&);
};

}
}
}
