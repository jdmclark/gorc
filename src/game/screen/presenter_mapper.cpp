#include "presenter_mapper.h"
#include "game/screen/action/action_presenter.h"

gorc::game::screen::presenter_mapper::presenter_mapper_place_visitor::presenter_mapper_place_visitor(gorc::game::components& components)
	: components(components) {
	return;
}

void gorc::game::screen::presenter_mapper::presenter_mapper_place_visitor::visit_action_place(const action::action_place& place) {
	presenter = std::unique_ptr<gorc::place::presenter>(new action::action_presenter(components, place));
}

std::unique_ptr<gorc::place::presenter> gorc::game::screen::presenter_mapper::get_presenter(const place& place) {
	presenter_mapper_place_visitor v(*components);
	place.accept(v);
	return std::move(v.presenter);
}
