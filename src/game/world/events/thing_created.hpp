#pragma once

#include "libold/base/utility/entity_id.hpp"
#include "libold/content/assets/template.hpp"

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
