#pragma once

#include "time.h"

namespace gorc {
inline namespace utility {

class aspect {
public:
    virtual ~aspect();
    virtual void update(gorc::time time) = 0;
    virtual void draw(gorc::time time) = 0;
};

}
}
