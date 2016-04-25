#pragma once

#include "utility/time.hpp"

namespace gorc {
inline namespace utility {

class aspect {
public:
    virtual ~aspect();
    virtual void update(time_delta time);
    virtual void draw(time_delta time);
};

}
}
