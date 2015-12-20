#pragma once

#include <sstream>
#include "utility/runtime_assert.hpp"

namespace gorc {

    template <typename RangeT>
    std::string join(RangeT const &list, std::string const &connective)
    {
        if(list.empty()) {
            return std::string();
        }

        std::stringstream sstream;

        auto curr_it = list.begin();
        sstream << *curr_it;
        ++curr_it;

        while(curr_it != list.end()) {
            sstream << connective << *curr_it;
            ++curr_it;
        }

        return sstream.str();
    }

}
