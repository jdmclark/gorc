#include "rate_limited_progress.hpp"

gorc::rate_limited_progress::rate_limited_progress(progress &prog, std::chrono::nanoseconds span)
    : prog(prog)
    , span(span)
{
    return;
}

void gorc::rate_limited_progress::advance(size_t steps_to_advance)
{
    queued_steps += steps_to_advance;

    auto next = clock::now();
    if((next - curr) >= span) {
        curr = next;
        prog.advance(queued_steps);
        queued_steps = 0;
    }
}

void gorc::rate_limited_progress::finished()
{
    if(queued_steps) {
        prog.advance(queued_steps);
    }

    prog.finished();
}
