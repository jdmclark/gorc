#include "presenter_mapper.h"
#include "places/action/action_presenter.h"

gorc::client::presenter_mapper::presenter_mapper(application& app) : app(app) {
    return;
}

std::unique_ptr<gorc::client::presenter> gorc::client::presenter_mapper::get_presenter(const action::action_place&) const {
    return make_unique<action::action_presenter>(app);
}
