#pragma once

#include "framework/view.h"

namespace gorc {
namespace client {
namespace action {

class action_presenter;

class action_view : public gorc::view {
private:
    maybe<action_presenter*> presenter;

public:
    action_view();

    virtual maybe<input::input_adapter*> get_input_adapter() override;

    inline void set_presenter(maybe<action_presenter*>&& presenter) {
        this->presenter = std::move(presenter);
    }
};

}
}
}
