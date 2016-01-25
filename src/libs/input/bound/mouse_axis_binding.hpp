#pragma once

#include "input_binding.hpp"
#include "axis_bindable_command.hpp"
#include "math/vector.hpp"

namespace gorc {

    class mouse_axis_binding : public input_binding {
    private:
        axis_bindable_command &command;
        vector<2, float> direction;

    public:
        mouse_axis_binding(axis_bindable_command &command, vector<2, float> const &direction);

        virtual void handle_mouse_velocity(time_delta, vector<2, float> const &velocity) override;
    };

}
