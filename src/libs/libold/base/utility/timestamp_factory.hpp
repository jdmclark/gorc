#pragma once

#include "timestamp.hpp"

namespace gorc {
inline namespace utility {

class timestamp_factory {
public:
    virtual ~timestamp_factory();

    virtual timestamp make_timestamp() = 0;
};

}
}
