#include "time.h"

gorc::utility::timestamp::timestamp(uint32_t ts_ms) : ts_ms(ts_ms) {
    return;
}

gorc::utility::time::time(uint32_t current_ts_ms, double elapsed_time)
    : current_ts_ms(current_ts_ms), elapsed_time(elapsed_time) {
    return;
}
