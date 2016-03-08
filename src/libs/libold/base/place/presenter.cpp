#include "presenter.hpp"

gorc::place::presenter::~presenter() {
    return;
}

bool gorc::place::presenter::can_stop() {
    return true;
}

void gorc::place::presenter::stop() {
    return;
}
