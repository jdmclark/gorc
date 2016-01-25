#pragma once

#include "button_bindable_command.hpp"
#include <functional>

namespace gorc {

    class repeat_button_bindable_command : public button_bindable_command {
    private:
        std::function<void(time_delta)> on_press;
        bool prev_state = false;
        bool curr_state = false;

        double repeat_delay;
        double repeat_time;
        double current_time = 0.0;

    public:
        repeat_button_bindable_command(double repeat_delay,
                                       double repeat_time,
                                       std::function<void(time_delta)> &&on_press);

        virtual void set_pressed() override;
        virtual void update(time_delta) override;
    };

}
