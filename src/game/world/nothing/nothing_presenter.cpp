#include "nothing_presenter.h"

gorc::game::world::nothing::nothing_presenter::nothing_presenter(class components& components)
	: components(components) {
	return;
}

void gorc::game::world::nothing::nothing_presenter::start(event::event_bus& eventBus) {
	components.world_view_frame.set_view(components.nothing_view);
}
