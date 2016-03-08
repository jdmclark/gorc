#pragma once

#include <memory>
#include "utility/event_bus.hpp"
#include "utility/maybe.hpp"

namespace gorc {
namespace place {

template <typename PresenterT, typename MapperT> class place_controller {
private:
    event_bus& bus;
    const MapperT& mapper;

    std::unique_ptr<PresenterT> curr_presenter;

public:
    place_controller(event_bus& bus, const MapperT& mapper)
        : bus(bus), mapper(mapper) {
        return;
    }

    template <typename PlaceT> void go_to(const PlaceT& place) {
        if(curr_presenter) {
            if(!curr_presenter->can_stop()) {
                // Abort
                return;
            }

            curr_presenter->stop();
        }

        curr_presenter = mapper.get_presenter(place);

        if(curr_presenter) {
            curr_presenter->start(bus);
        }
    }

    maybe<PresenterT*> current_presenter() const {
        return curr_presenter.get();
    }
};

}
}
