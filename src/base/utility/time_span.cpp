#include "time_span.h"

gorc::utility::time_span::time_span(timestamp begin, timestamp end)
    : begin(begin), end(end),
      elapsed((static_cast<double>(end.as_milliseconds()) - static_cast<double>(begin.as_milliseconds())) / 1000.0) {
    return;
}
