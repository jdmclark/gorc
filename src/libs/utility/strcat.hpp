#pragma once

#include <sstream>
#include "foreach.hpp"

namespace gorc {

    struct strcat_op_impl {
    private:
        std::ostream &os;

    public:
        strcat_op_impl(std::ostream &os);

        template <typename T>
        void operator()(T const &v)
        {
            os << v;
        }
    };

    template <typename ...T>
    std::string strcat(T const &...v)
    {
        std::ostringstream os;

        // TODO: Replace the following with a C++17 fold expression
        strcat_op_impl strcat_op(os);
        foreach(strcat_op, v...);

        return os.str();
    }

}
