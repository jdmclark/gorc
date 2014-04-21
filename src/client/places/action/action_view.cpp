#include "action_view.h"
#include "action_presenter.h"
#include <iostream>

gorc::client::action::action_view::action_view() {
    return;
}

gorc::maybe<gorc::input::input_adapter*> gorc::client::action::action_view::get_input_adapter() {
    if(action_presenter* p = presenter) {
        return make_maybe(&p->get_input_adapter());
    }

    return nothing;
}
