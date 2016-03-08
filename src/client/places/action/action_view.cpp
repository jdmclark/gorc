#include "action_view.hpp"
#include "action_presenter.hpp"
#include <iostream>

gorc::client::action::action_view::action_view() {
    return;
}

gorc::maybe<gorc::input::input_adapter*> gorc::client::action::action_view::get_input_adapter() {
    return maybe_if(presenter, nothing, [](action_presenter *p) {
        return make_maybe(&p->get_input_adapter());
    });
}
