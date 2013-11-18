#include "application.h"
#include "world/level_presenter.h"
#include "world/level_model.h"
#include "world/level_view.h"
#include <boost/algorithm/string/predicate.hpp>

gorc::game::application::application(content::vfs::virtual_filesystem& vfs, diagnostics::report& report,
		const std::string& episode_name, const std::string& level_name)
	: gorc::application<view_layer, presenter, presenter_mapper>("Gorc", mapper, report, vfs, make_box(make_vector(0, 0), make_vector(1280, 720)), true),
	  mapper(*this), input_episodename(episode_name), input_levelname(level_name), virtual_filesystem(vfs), compiler(verb_table) {
	return;
}

gorc::game::application::~application() {
	return;
}

void gorc::game::application::startup(event::event_bus& event_bus, content::manager& content) {
	level_view = make_unique<world::level_view>(content);
	action_view = make_unique<action::action_view>();

	// Register verbs
	world::level_presenter::register_verbs(verb_table, *this);

	views.set_layer(view_layer::clear_screen, clear_view);
	views.set_layer(view_layer::world, *level_view);

	// HACK: Set current episode to The Force Within.
	for(size_t i = 0; i < virtual_filesystem.get_episode_count(); ++i) {
		if(boost::iequals(virtual_filesystem.get_episode(i).get_episode_name(), input_episodename)) {
			virtual_filesystem.set_episode(i);
		}
	}

	// HACK: Set current level to 01narshadda.jkl.
	auto contentmanager = std::make_shared<content::manager>(report, filesystem);
	const auto& lev = contentmanager->load<content::assets::level>(input_levelname, compiler);

	// Start game:
	current_level_presenter = make_unique<world::level_presenter>(*this, world::level_place(contentmanager, lev));
	place_controller.go_to(action::action_place());

	current_level_presenter->start(event_bus);

	return;
}

void gorc::game::application::shutdown() {
	return;
}

void gorc::game::application::update(const time& time, const box<2, int>& view_size) {
	current_level_presenter->update(time);
}
