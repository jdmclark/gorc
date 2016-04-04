#include "touched_surface.hpp"

gorc::game::world::events::touched_surface::touched_surface(int toucher, int surface)
    : toucher(toucher), touched(surface) {
    return;
}
