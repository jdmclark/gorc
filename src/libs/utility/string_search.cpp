#include "string_search.hpp"

bool gorc::begins_with(std::string const &searched, std::string const &prefix)
{
    auto it = searched.begin();
    auto jt = prefix.begin();

    while(it != searched.end() &&
          jt != prefix.end() &&
          *it == *jt)
    {
        ++it;
        ++jt;
    }

    return jt == prefix.end();
}
