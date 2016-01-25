#pragma once

#include "button_bindable_command.hpp"
#include <functional>

namespace gorc {

    class repeat_button_bindable_command : public button_bindable_command {
    private:
        std::function<void(time_delta)> on_press;
        bool prev_state = false;
        bool curr_state = false;

        time_delta repeat_delay;
        time_delta repeat_time;
        time_delta current_time = 0.0s;

    public:
        repeat_button_bindable_command(time_delta repeat_delay,
                                       time_delta repeat_time,
                                       std::function<void(time_delta)> &&on_press);

        virtual void set_pressed() override;
        virtual void update(time_delta) override;
    };

}
