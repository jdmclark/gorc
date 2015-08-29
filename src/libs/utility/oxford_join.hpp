#pragma once

#include <sstream>
#include "utility/runtime_assert.hpp"

namespace gorc {

    template <typename RangeT>
    std::string oxford_join(RangeT const &list,
                            std::string const &connective = "and")
    {
        runtime_assert(!list.empty(), "cannot format empty list");

        std::stringstream sstream;

        auto lookahead_it = list.begin();
        auto curr_it = lookahead_it++;

        // Write out first word, which is always valid.
        sstream << *curr_it;

        // Advance if possible
        if(lookahead_it == list.end()) {
            return sstream.str();
        }

        curr_it = lookahead_it++;

        // Special case: If the second word is the last word, output without comma.
        if(lookahead_it == list.end()) {
            sstream << " " << connective << " " << *curr_it;
            return sstream.str();
        }

        // List has at least 3 elements
        while(lookahead_it != list.end()) {
            sstream << ", " << *curr_it;
            curr_it = lookahead_it++;
        }

        // Output the last element
        sstream << ", " << connective << " " << *curr_it;

        return sstream.str();
    }

}
