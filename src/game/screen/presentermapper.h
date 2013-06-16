#pragma once

#include "place.h"
#include "framework/place/presentermapper.h"
#include <memory>

namespace Gorc {
namespace Game {

class Components;

namespace Screen {

class PresenterMapper : public Gorc::Place::PresenterMapper<Place> {
private:
	class PresenterMapperPlaceVisitor : public PlaceVisitor {
	private:
		Components& components;

	public:
		std::unique_ptr<Gorc::Place::Presenter> presenter;

		PresenterMapperPlaceVisitor(Components& components);

		void VisitActionPlace(const Action::ActionPlace& place);
	};

	Components* components;

public:
	inline void SetComponents(Components& cmp) {
		components = &cmp;
	}

	std::unique_ptr<Gorc::Place::Presenter> GetPresenter(const Place& place);
};

}
}
}
