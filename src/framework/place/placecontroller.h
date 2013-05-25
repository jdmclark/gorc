#pragma once

#include <memory>
#include "presentermapper.h"
#include "framework/event/eventbus.h"

namespace Gorc {
namespace Place {

template <typename T> class PlaceController {
private:
	Event::EventBus& eventBus;
	PresenterMapper<T>& presenterMapper;

	std::unique_ptr<Event::EventBus> currentChildEventBus;
	std::unique_ptr<Presenter> currentPresenter;

public:
	PlaceController(Event::EventBus& eventBus, PresenterMapper<T>& presenterMapper)
		: eventBus(eventBus), presenterMapper(presenterMapper) {
		return;
	}

	void GoTo(const T& place) {
		if(currentPresenter) {
			if(!currentPresenter->CanStop()) {
				// Abort
				return;
			}

			currentPresenter->Stop();
		}

		currentChildEventBus = std::unique_ptr<Event::EventBus>(new Event::EventBus(&eventBus));
		currentPresenter = presenterMapper.GetPresenter(place);

		if(currentPresenter) {
			currentPresenter->Start(*currentChildEventBus);
		}
	}
};

}
}
