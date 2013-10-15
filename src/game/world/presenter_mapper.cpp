#include "presenter_mapper.h"
#include "game/world/nothing/nothing_presenter.h"
#include "game/world/level/level_presenter.h"

gorc::game::world::presenter_mapper::presenter_mapper_place_visitor::presenter_mapper_place_visitor(gorc::game::components& components)
	: components(components) {
	return;
}

void gorc::game::world::presenter_mapper::presenter_mapper_place_visitor::visit_nothing_place(const nothing::nothing_place& place) {
	presenter = std::unique_ptr<gorc::place::presenter>(new nothing::nothing_presenter(components));
}

void gorc::game::world::presenter_mapper::presenter_mapper_place_visitor::visit_level_place(const level::level_place& place) {
	presenter = std::unique_ptr<gorc::place::presenter>(new level::level_presenter(components, place));
}

std::unique_ptr<gorc::place::presenter> gorc::game::world::presenter_mapper::get_presenter(const place& place) {
	presenter_mapper_place_visitor v(*components);
	place.accept(v);
	return std::move(v.presenter);
}
