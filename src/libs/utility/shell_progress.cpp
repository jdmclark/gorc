#include "shell_progress.hpp"
#include "make_unique.hpp"
#include <vector>

namespace {

    std::vector<char const *> spinner_states {
        "-",
        "\\",
        "|",
        "/"
    };

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
    stream << "\r[";

    size_t max_num = width - 2;
    size_t num_filled = (current * max_num) / steps;

    size_t i = 1;
    for(; i < num_filled; ++i) {
        stream << "-";
    }

    if(draw_spinner) {
        size_t spindex = (current % spinner_states.size());
        stream << spinner_states.at(spindex);
    }
    else {
        stream << "-";
    }

    // Fill rest
    for(; i < max_num; ++i) {
        stream << " ";
    }

    stream << "]";
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
