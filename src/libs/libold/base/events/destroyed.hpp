#pragma once

#include "content/id.hpp"

namespace gorc {
namespace events {

class destroyed {
public:
    #include "destroyed.uid"

    thing_id destroyed_entity;

    destroyed(thing_id destroyed_entity);
};

}
}
