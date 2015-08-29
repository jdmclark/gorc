#pragma once

#include <string>

namespace test {

    template <void(*fn)(const std::string&, const std::string&, int)>
    class autorun {
    public:
        autorun(const std::string& s, const std::string& f, int ln)
        {
            fn(s, f, ln);
            return;
        }
    };

}
