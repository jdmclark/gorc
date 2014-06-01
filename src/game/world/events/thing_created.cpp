#include "thing_created.h"

gorc::game::world::events::thing_created::thing_created(entity_id thing,
                                                        content::assets::thing_template const &tpl)
    : thing(thing), tpl(tpl) {
    return;
}
