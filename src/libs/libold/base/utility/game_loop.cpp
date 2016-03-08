#include "game_loop.hpp"
#include "time_span.hpp"

gorc::utility::game_loop::game_loop(timestamp_factory& tf, uint32_t ms_per_update) : tf(tf), ms_per_update(ms_per_update) {
    return;
}

gorc::utility::game_loop::~game_loop() {
    return;
}

void gorc::utility::game_loop::run() {
    is_running = true;

    timestamp prev_draw_time(0);
    timestamp prev_update_time(0);

    while(is_running) {
        timestamp curr_time = tf.make_timestamp();

        draw(time(curr_time, prev_draw_time));

        prev_draw_time = curr_time;

        while((curr_time.as_milliseconds() - prev_update_time.as_milliseconds()) >= ms_per_update) {
            update(time(timestamp(prev_update_time.as_milliseconds() + ms_per_update), prev_update_time));
            prev_update_time = timestamp(prev_update_time.as_milliseconds() + ms_per_update);
        }
    }
}

void gorc::utility::game_loop::stop() {
    is_running = false;
}
