#pragma once

#include "libold/base/place/presenter.hpp"
#include "libold/base/utility/time.hpp"

namespace gorc {

class application_presenter : public place::presenter {
public:
    virtual ~application_presenter();

    virtual void update(const gorc::time& time);
};

}
