#pragma once

#include "presenter.h"
#include "places/action/action_place.h"

namespace gorc {
namespace game {

class application;

class presenter_mapper {
private:
	application& app;

public:
	presenter_mapper(application& app);

	std::unique_ptr<presenter> get_presenter(const action::action_place&) const;
};

}
}
