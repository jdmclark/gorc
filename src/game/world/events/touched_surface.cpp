#include "touched_surface.hpp"

gorc::game::world::events::touched_surface::touched_surface(thing_id toucher, surface_id surface)
    : toucher(toucher), touched(surface) {
    return;
}
