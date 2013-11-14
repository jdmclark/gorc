#include "presenter_mapper.h"
#include "places/action/action_presenter.h"

gorc::game::presenter_mapper::presenter_mapper(application& app) : app(app) {
	return;
}

std::unique_ptr<gorc::game::presenter> gorc::game::presenter_mapper::get_presenter(const action::action_place&) const {
	return make_unique<action::action_presenter>(app);
}
