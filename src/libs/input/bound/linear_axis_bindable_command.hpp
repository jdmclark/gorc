#pragma once

#include "axis_bindable_command.hpp"
#include <functional>

namespace gorc {

    class linear_axis_bindable_command : public axis_bindable_command {
    private:
        std::function<void(time_delta, float)> on_update;
        float value = 0.0f;

    public:
        linear_axis_bindable_command(std::function<void(time_delta, float)> &&on_update);

        virtual void add_value(float dv) override;
        virtual void update(time_delta) override;
    };

}
