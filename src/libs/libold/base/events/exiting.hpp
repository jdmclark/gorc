#pragma once

#include <cstdint>

namespace gorc {
namespace events {

class exiting {
public:
    #include "exiting.uid"

    bool cancel = false;
};

}
}
