#pragma once

#include "framework/event/event_bus.h"

namespace gorc {
namespace place {

class presenter {
public:
    virtual ~presenter();

    virtual void start(event::event_bus& eventBus) = 0;

    virtual bool can_stop();
    virtual void stop();
};

}
}
