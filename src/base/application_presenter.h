#pragma once

#include "base/place/presenter.h"
#include "base/utility/time.h"

namespace gorc {

class application_presenter : public place::presenter {
public:
    virtual ~application_presenter();

    virtual void update(const time& time);
};

}
