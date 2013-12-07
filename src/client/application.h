#pragma once

#include "framework/application.h"
#include "view_layer.h"
#include "presenter.h"
#include "presenter_mapper.h"
#include "clear_screen_view.h"
#include "places/action/action_view.h"
#include "content/vfs/virtual_filesystem.h"
#include "game/level_state.h"

#include "framework/utility/randomizer.h"
#include "cog/verbs/table.h"
#include "cog/compiler.h"

namespace gorc {
namespace client {

namespace world {
class level_view;
class level_presenter;
}

class application : public gorc::application<view_layer, presenter, presenter_mapper> {
private:
	presenter_mapper mapper;
	clear_screen_view clear_view;

	void register_verbs();

public:
	const std::string input_episodename;
	const std::string input_levelname;

	content::vfs::virtual_filesystem& virtual_filesystem;

	utility::randomizer randomizer;

	std::unique_ptr<world::level_view> level_view;
	std::unique_ptr<action::action_view> action_view;

	game::level_state components;

	application(content::vfs::virtual_filesystem& filesystem, diagnostics::report& report,
			const std::string& input_episodename, const std::string& input_levelname);
	~application();

	virtual void startup(event::event_bus& event_bus, content::manager& content) override;
	virtual void shutdown() override;

	virtual void update(const time& time, const box<2, int>& view_size) override;
};

}
}
