#pragma once

#include <memory>
#include "presenter_mapper.h"
#include "framework/event/event_bus.h"

namespace gorc {
namespace place {

template <typename T> class place_controller {
private:
	event::event_bus& eventBus;
	presenter_mapper<T>& presenterMapper;

	std::unique_ptr<event::event_bus> currentChildEventBus;
	std::unique_ptr<presenter> currentPresenter;

public:
	place_controller(event::event_bus& eventBus, presenter_mapper<T>& presenterMapper)
		: eventBus(eventBus), presenterMapper(presenterMapper) {
		return;
	}

	void go_to(const T& place) {
		if(currentPresenter) {
			if(!currentPresenter->can_stop()) {
				// Abort
				return;
			}

			currentPresenter->stop();
		}

		currentChildEventBus = std::unique_ptr<event::event_bus>(new event::event_bus(&eventBus));
		currentPresenter = presenterMapper.get_presenter(place);

		if(currentPresenter) {
			currentPresenter->start(*currentChildEventBus);
		}
	}
};

}
}
