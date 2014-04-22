#include "time.h"

gorc::utility::time::time(gorc::utility::timestamp current, gorc::utility::timestamp prev)
    : current(current),
      elapsed_span(prev, current) {
    return;
}
