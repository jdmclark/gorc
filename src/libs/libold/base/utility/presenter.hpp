#pragma once

#include "utility/event_bus.hpp"

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
