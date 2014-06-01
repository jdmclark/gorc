#pragma once

#include "base/utility/entity_id.h"
#include "content/assets/template.h"

namespace gorc {
namespace game {
namespace world {
namespace events {

class thing_created {
public:
    #include "thing_created.uid"

    entity_id thing;
    content::assets::thing_template const &tpl;

    thing_created(entity_id thing, content::assets::thing_template const &tpl);
};

}
}
}
}
