#pragma once

#include "time.h"
#include "timestamp_factory.h"

namespace gorc {
inline namespace utility {

class game_loop {
private:
    timestamp_factory& tf;
    bool is_running = false;
    uint32_t ms_per_update;

public:
    game_loop(timestamp_factory& tf, uint32_t ms_per_update = 15U);
    virtual ~game_loop();

    void run();
    void stop();

    virtual void update(time t) = 0;
    virtual void draw(time t) = 0;
};

}
}
