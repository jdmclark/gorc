#pragma once

#include "timestamp.h"

namespace gorc {
inline namespace utility {

class timestamp_factory {
public:
    virtual ~timestamp_factory();

    virtual timestamp make_timestamp() = 0;
};

}
}
