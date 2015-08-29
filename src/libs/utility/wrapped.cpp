#include "wrapped.hpp"
#include "utility/range.hpp"

namespace gorc {

    template <typename RangeT>
    RangeT internal_get_word(RangeT const &rng)
    {
        auto word_begin = std::begin(rng);

        // Scan to start of word.
        while((word_begin != std::end(rng)) &&
              (std::isspace(*word_begin))) {
            ++word_begin;
        }

        auto word_end = word_begin;

        // Scan to end of word.
        while((word_end != std::end(rng)) &&
              (!std::isspace(*word_end))) {
            ++word_end;
        }

        return make_range(word_begin, word_end);
    }

}

std::string gorc::wrapped(std::string const &msg, size_t width)
{
    return wrapped("", msg, width);
}

std::string gorc::wrapped(std::string const &tag,
                          std::string const &msg,
                          size_t width)
{
    std::string output;

    size_t margin = 0;
    bool needs_margin = false;

    if(!tag.empty()) {
        margin = tag.size() + 3;
        output.append("[");
        output.append(tag);
        output.append("] ");
    }

    auto line_begin = std::begin(msg);
    auto line_end = line_begin;
    while(line_begin != std::end(msg)) {
        auto next_word_rng = internal_get_word(make_range(line_end, std::end(msg)));
        auto new_line_length = static_cast<size_t>(std::end(next_word_rng) - line_begin);

        if((new_line_length > (width - margin)) ||
           (std::begin(next_word_rng) == std::end(msg))) {
            if(needs_margin) {
                output.push_back('\n');
                for(size_t i = 0; i < margin; ++i) {
                    output.push_back(' ');
                }
            }

            needs_margin = true;

            bool contains_newline = false;
            for( ; line_begin != line_end; ++line_begin) {
                if(*line_begin == '\n') {
                    ++line_begin;
                    contains_newline = true;
                    break;
                }
                else {
                    output.push_back(*line_begin);
                }
            }

            if(!contains_newline) {
                line_begin = std::begin(next_word_rng);
                line_end = std::end(next_word_rng);
            }
        }
        else {
            line_end = std::end(next_word_rng);
        }
    }

    return output;
}
