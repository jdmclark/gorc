#include "component_system.hpp"

gorc::utility::component_system::component_system(event_bus &bus)
    : bus(bus)
{
    return;
}

gorc::utility::component_system::entity_iterator::entity_iterator(const pool<entity, 1024>::iterator& it) : it(it) {
    return;
}

gorc::utility::component_system::entity_const_iterator::entity_const_iterator(const pool<entity, 1024>::const_iterator& it) : it(it) {
    return;
}

gorc::utility::component_system::pool_container::~pool_container() {
    return;
}

void gorc::utility::component_system::update(gorc::time time) {
    for(auto& aspect : aspects) {
        aspect->update(time);
    }
}

void gorc::utility::component_system::draw(gorc::time time) {
    for(auto& aspect : aspects) {
        aspect->draw(time);
    }
}
