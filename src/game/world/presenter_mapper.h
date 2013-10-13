#pragma once

#include "place.h"
#include "framework/place/presenter_mapper.h"
#include <memory>

namespace Gorc {
namespace Game {

class Components;

namespace World {

class PresenterMapper : public Gorc::Place::PresenterMapper<Place> {
private:
	class PresenterMapperPlaceVisitor : public PlaceVisitor {
	private:
		Components& components;

	public:
		std::unique_ptr<Gorc::Place::Presenter> presenter;

		PresenterMapperPlaceVisitor(Components& components);

		void VisitNothingPlace(const Nothing::NothingPlace& place);
		void VisitLevelPlace(const Level::LevelPlace& place);
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
