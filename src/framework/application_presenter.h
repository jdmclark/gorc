#pragma once

#include "framework/place/presenter.h"
#include "framework/utility/time.h"

namespace gorc {

class application_presenter : public place::presenter {
public:
	virtual ~application_presenter();

	virtual void update(const time& time);
};

}
