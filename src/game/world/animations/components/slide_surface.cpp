#include "slide_surface.hpp"

gorc::game::world::animations::components::slide_surface::slide_surface(entity_id surface, const vector<3>& direction,
        const vector<3>& sb0, const vector<3>& sb1, const vector<2>& tb0, const vector<2>& tb1)
    : surface(surface), direction(direction), sb0(sb0), sb1(sb1), tb0(tb0), tb1(tb1) {
    return;
}
