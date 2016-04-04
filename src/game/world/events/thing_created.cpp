#include "thing_created.hpp"

gorc::game::world::events::thing_created::thing_created(int thing,
                                                        content::assets::thing_template const &tpl)
    : thing(thing), tpl(tpl) {
    return;
}
