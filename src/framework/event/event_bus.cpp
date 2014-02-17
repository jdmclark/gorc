#include "event_bus.h"
#include <algorithm>
#include <iostream>

using namespace gorc::event;

event_bus::event_bus_handler_container::~event_bus_handler_container() {
    return;
}

void event_bus::add_child(event_bus* child) {
    children.push_back(child);
}

void event_bus::remove_child(event_bus* child) {
    children.erase(std::remove_if(children.begin(), children.end(), [&child](event_bus* em) { return em == child; }), children.end());
}

event_bus::event_bus() : parent(nullptr) {
    return;
}

event_bus::event_bus(event_bus* parent) : parent(parent) {
    parent->add_child(this);
    return;
}

event_bus::~event_bus() {
    if(parent != nullptr) {
        parent->remove_child(this);
    }
}
