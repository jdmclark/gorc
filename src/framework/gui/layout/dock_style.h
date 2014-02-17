#pragma once

#include "framework/utility/flag_set.h"

namespace gorc {
namespace gui {
namespace layout {

enum class dock_style {
    nothing = 0,

    width = 0x1,
    height = 0x2,
    fill = width | height
};

}
}
}
