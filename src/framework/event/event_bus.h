#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include "framework/utility/make_unique.h"

namespace gorc {
namespace event {

class event_bus {
private:
    class event_bus_handler_container {
    public:
        virtual ~event_bus_handler_container();
    };

    template <typename T> class event_bus_handler_container_impl : public event_bus_handler_container {
    public:
        std::vector<std::function<void(T&)>> handlers;
    };

    event_bus* parent;
    std::unordered_map<std::string, std::unique_ptr<event_bus_handler_container>> handlers;
    std::vector<event_bus*> children, children_scratch;

    void add_child(event_bus* child);
    void remove_child(event_bus* child);

    template <typename T> std::vector<std::function<void(T&)>>& get_handler_vector() {
        auto type = T::get_event_type();
        auto it = handlers.find(type);
        if(it == handlers.end()) {
            return reinterpret_cast<event_bus_handler_container_impl<T>*>(std::get<0>(handlers.emplace(type,
                    make_unique<event_bus_handler_container_impl<T>>()))->second.get())->handlers;
        }
        else {
            return reinterpret_cast<event_bus_handler_container_impl<T>*>(it->second.get())->handlers;
        }
    }

    template <typename T> void dispatch_event(T& event) {
        auto& hvec = get_handler_vector<T>();
        for(auto& eh : hvec) {
            eh(event);
        }

        children_scratch = children;

        for(auto child : children_scratch) {
            child->dispatch_event(event);
        }
    }

public:
    event_bus();
    explicit event_bus(event_bus* parent);
    ~event_bus();

    template <typename T> void fire_event(T& event) {
        if(parent == nullptr) {
            dispatch_event(event);
            return;
        }

        parent->dispatch_event(event);
    }

    template <typename T> void fire_event(const T& event) {
        if(parent == nullptr) {
            dispatch_event(event);
            return;
        }

        parent->dispatch_event(event);
    }

    template <typename T> void add_handler(std::function<void(T&)> handler) {
        auto& hvec = get_handler_vector<T>();
        hvec.push_back(handler);
    }

    event_bus(const event_bus&) = delete;
    event_bus& operator=(const event_bus&) = delete;
};

}
}
