#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

namespace Gorc {
namespace Event {

class EventBus {
private:
	class EventBusHandlerContainer {
	public:
		virtual ~EventBusHandlerContainer();
	};

	template <typename T> class EventBusHandlerContainerImpl : public EventBusHandlerContainer {
	public:
		std::vector<std::function<void(T&)>> handlers;
	};

	EventBus* parent;
	std::unordered_map<std::string, EventBusHandlerContainer*> handlers;
	std::vector<EventBus*> children;

	void AddChild(EventBus* child);
	void RemoveChild(EventBus* child);

	template <typename T> std::vector<std::function<void(T&)>>& GetHandlerVector() {
		auto type = T::GetEventType();
		auto it = handlers.find(type);
		if(it == handlers.end()) {
			auto nv = new EventBusHandlerContainerImpl<T>();
			handlers.insert(std::make_pair(type, nv));
			return nv->handlers;
		}
		else {
			auto hc = reinterpret_cast<EventBusHandlerContainerImpl<T>*>(it->second);
			return hc->handlers;
		}
	}

	template <typename T> void DispatchEvent(T& event) {
		auto& hvec = GetHandlerVector<T>();
		for(auto& eh : hvec) {
			eh(event);
		}

		std::vector<EventBus*> tchildren(children);

		for(auto child : tchildren) {
			child->DispatchEvent(event);
		}
	}

public:
	EventBus();
	EventBus(EventBus* parent);
	~EventBus();

	template <typename T> void FireEvent(T& event) {
		if(parent == nullptr) {
			DispatchEvent(event);
			return;
		}

		parent->DispatchEvent(event);
	}

	template <typename T> void AddHandler(std::function<void(T&)> handler) {
		auto& hvec = GetHandlerVector<T>();
		hvec.push_back(handler);
	}

	EventBus(const EventBus&) = delete;
	EventBus& operator=(const EventBus&) = delete;
};

}
}
