#pragma once

#include "framework/math/util.h"

namespace gorc {
inline namespace utility {

template <typename T, typename DeltaT = float> class linear_eased {
private:
    T initial_value;
    T end_value;
    T current_value;
    DeltaT current_time;
    DeltaT goal_time;

public:
    void update(DeltaT dt) {
        current_time += dt;

        if(goal_time <= DeltaT(0)) {
            current_value = end_value;
        }
        else {
            current_value = lerp(initial_value, end_value, clamp(current_time / goal_time, DeltaT(0), DeltaT(1)));
        }
    }

    operator T() const {
        return current_value;
    }

    void set(T goal_value, DeltaT delay) {
        initial_value = current_value;
        end_value = goal_value;
        current_time = 0;
        goal_time = delay;
        update(DeltaT(0));
    }
};

}
}
