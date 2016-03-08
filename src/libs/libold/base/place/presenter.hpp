#pragma once

#include "utility/event_bus.hpp"

namespace gorc {
namespace place {

class presenter {
public:
    virtual ~presenter();

    virtual void start(event_bus& eventBus) = 0;

    virtual bool can_stop();
    virtual void stop();
};

}
}
