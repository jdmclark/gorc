#pragma once

#include "event_bus.h"

namespace gorc {
inline namespace utility {

class presenter {
public:
    virtual ~presenter();

    virtual void start(event_bus&) = 0;
    virtual bool can_stop() const;
    virtual void stop();
};

}
}
