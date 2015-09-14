#pragma once

#include <string>

namespace gorc {

    bool begins_with(std::string const &searched, std::string const &prefix);

    template <typename LeftRngT, typename RightRngT>
    bool iequal(LeftRngT const &left, RightRngT const &right)
    {
        auto it = left.begin();
        auto jt = right.begin();

        while(it != left.end() &&
              jt != right.end() &&
              tolower(*it) == tolower(*jt)) {
            ++it;
            ++jt;
        }

        return it == left.end() &&
               jt == right.end();
    }

}
