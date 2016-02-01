#pragma once

#include <chrono>
#include "progress.hpp"

namespace gorc {

    class rate_limited_progress : public progress {
    private:
        progress &prog;

        using clock = std::chrono::steady_clock;
        clock::time_point curr = clock::now();
        std::chrono::nanoseconds span;
        size_t queued_steps = 0;

    public:
        rate_limited_progress(progress &prog,
                              std::chrono::nanoseconds span = std::chrono::milliseconds(20));

        virtual void advance(size_t steps_to_advance = 1) override;
        virtual void finished() override;
    };

}
