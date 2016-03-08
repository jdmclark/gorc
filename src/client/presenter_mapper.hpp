#pragma once

#include "presenter.hpp"
#include "places/action/action_place.hpp"

namespace gorc {
namespace client {

class application;

class presenter_mapper {
private:
    application& app;

public:
    presenter_mapper(application& app);

    std::unique_ptr<presenter> get_presenter(const action::action_place&) const;
};

}
}
