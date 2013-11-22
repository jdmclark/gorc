#pragma once

#include <memory>
#include "framework/event/event_bus.h"
#include "framework/utility/maybe.h"

namespace gorc {
namespace place {

template <typename PresenterT, typename MapperT> class place_controller {
private:
	event::event_bus& bus;
	const MapperT& mapper;

	std::unique_ptr<event::event_bus> current_child_bus;
	std::unique_ptr<PresenterT> curr_presenter;

public:
	place_controller(event::event_bus& bus, const MapperT& mapper)
		: bus(bus), mapper(mapper) {
		return;
	}

	template <typename PlaceT> void go_to(const PlaceT& place) {
		if(curr_presenter) {
			if(!curr_presenter->can_stop()) {
				// Abort
				return;
			}

			curr_presenter->stop();
		}

		current_child_bus = make_unique<event::event_bus>(&bus);
		curr_presenter = mapper.get_presenter(place);

		if(curr_presenter) {
			curr_presenter->start(*current_child_bus);
		}
	}

	maybe<PresenterT*> current_presenter() const {
		return curr_presenter.get();
	}
};

}
}
