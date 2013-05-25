#include "eventbus.h"
#include <algorithm>
#include <iostream>

using namespace Gorc::Event;

EventBus::EventBusHandlerContainer::~EventBusHandlerContainer() {
	return;
}

void EventBus::AddChild(EventBus* child) {
	children.push_back(child);
}

void EventBus::RemoveChild(EventBus* child) {
	children.erase(std::remove_if(children.begin(), children.end(), [child](EventBus* em) { return em == child; }), children.end());
}

EventBus::EventBus() : parent(nullptr) {
	return;
}

EventBus::EventBus(EventBus* parent) : parent(parent) {
	parent->AddChild(this);
	return;
}

EventBus::~EventBus() {
	if(parent != nullptr) {
		parent->RemoveChild(this);
	}

	for(auto pair : handlers) {
		delete pair.second;
	}
}
