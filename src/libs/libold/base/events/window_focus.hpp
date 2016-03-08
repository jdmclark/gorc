#pragma once

#include <cstdint>

namespace gorc {
namespace events {

class window_focus {
public:
    #include "window_focus.uid"

    const bool has_focus;

    window_focus(bool HasFocus);
};

}
}
