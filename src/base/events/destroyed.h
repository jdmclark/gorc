#pragma once

#include "base/utility/entity_id.h"

namespace gorc {
namespace events {

class destroyed {
public:
    #include "destroyed.uid"

    entity_id destroyed_entity;

    destroyed(entity_id destroyed_entity);
};

}
}
