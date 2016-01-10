#include "shell_progress.hpp"
#include "make_unique.hpp"
#include <numeric>
#include <vector>

namespace {

    char const *line_start = "\r";
    char const *fill = u8"▪";
    char const *empty = u8"▫";

    std::vector<char const *> const spinner_states {
        u8"◐",
        u8"◓",
        u8"◑",
        u8"◒"
    };

    char const *dead_spinner = u8"●";

}

gorc::shell_progress::shell_progress(size_t steps, size_t width, std::ostream &str)
    : stream(str)
    , steps(steps)
    , width(width)
{
    advance(0);
}

void gorc::shell_progress::render(bool draw_spinner)
{
    stream << line_start;

    if(draw_spinner) {
        size_t spindex = (current % spinner_states.size());
        stream << spinner_states.at(spindex);
    }
    else {
        stream << dead_spinner;
    }

    stream << " ";

    size_t max_num = width - 2;
    size_t num_filled = steps ? ((std::min(current, steps) * max_num) / steps) : 0;

    size_t i = 1;
    for(; i < num_filled; ++i) {
        stream << fill;
    }

    // Fill rest
    for(; i < max_num; ++i) {
        stream << empty;
    }
}

void gorc::shell_progress::advance(size_t steps_to_advance)
{
    current += steps_to_advance;
    render(true);
}

void gorc::shell_progress::finished()
{
    render(false);
    stream << std::endl;
}

gorc::shell_progress_factory::shell_progress_factory(size_t width,
                                                     std::ostream &stream)
    : stream(stream)
    , width(width)
{
    return;
}

std::unique_ptr<gorc::progress> gorc::shell_progress_factory::make_progress(size_t steps)
{
    return make_unique<shell_progress>(steps, width, stream);
}
