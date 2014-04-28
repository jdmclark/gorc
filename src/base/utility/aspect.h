#pragma once

#include "time.h"

namespace gorc {
inline namespace utility {

class aspect {
public:
    virtual ~aspect();
    virtual void update(gorc::time time);
    virtual void draw(gorc::time time);
};

}
}
