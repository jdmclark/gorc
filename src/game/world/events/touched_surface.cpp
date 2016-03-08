#include "touched_surface.hpp"

gorc::game::world::events::touched_surface::touched_surface(entity_id toucher, int surface)
    : toucher(toucher), touched(surface) {
    return;
}
