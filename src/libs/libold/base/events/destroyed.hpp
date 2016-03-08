#pragma once

#include "libold/base/utility/entity_id.hpp"

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
