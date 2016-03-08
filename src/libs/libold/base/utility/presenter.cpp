#include "presenter.hpp"

gorc::utility::presenter::~presenter() {
    return;
}

bool gorc::utility::presenter::can_stop() const {
    return true;
}

void gorc::utility::presenter::stop() {
    return;
}
