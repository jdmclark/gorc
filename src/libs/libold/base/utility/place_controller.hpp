#pragma once

#include <memory>
#include "utility/event_bus.hpp"
#include "presenter.hpp"

namespace gorc {
inline namespace utility {

template <typename MapperT> class place_controller {
private:
    event_bus& bus;
    const MapperT& mapper;

    std::unique_ptr<event_bus> current_child_bus;
    std::unique_ptr<presenter> curr_presenter;

public:
    place_controller(event_bus& bus, const MapperT& mapper)
        : bus(bus), mapper(mapper) {
        return;
    }

    template <typename PlaceT, typename... ArgT> void go_to(ArgT&&... args) {
        if(curr_presenter) {
            if(!curr_presenter->can_stop()) {
                // Abort
                return;
            }

            curr_presenter->stop();
        }

        current_child_bus = std::make_unique<event_bus>(&bus);
        curr_presenter = mapper.get_presenter(std::make_unique<PlaceT>(std::forward<ArgT>(args)...));

        if(curr_presenter) {
            curr_presenter->start(*current_child_bus);
        }
    }
};

}
}
