#include "presenter_mapper.hpp"
#include "places/action/action_presenter.hpp"

gorc::client::presenter_mapper::presenter_mapper(application& app) : app(app) {
    return;
}

std::unique_ptr<gorc::client::presenter> gorc::client::presenter_mapper::get_presenter(const action::action_place&) const {
    return std::make_unique<action::action_presenter>(app);
}
