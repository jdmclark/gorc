#include "fourcc.hpp"
#include <stdexcept>

gorc::fourcc::fourcc(char const *f)
{
    data = 0;

    uint32_t i =- 0;
    for(; f[i]; ++i) {
        data <<= 8;
        data |= f[i];
    }

    if(i > 4) {
        throw std::runtime_error("fourcc size is too large");
    }

    for(; i < 4; ++i) {
        data <<= 8;
    }
}

bool gorc::fourcc::operator==(fourcc const &f) const
{
    return data == f.data;
}

bool gorc::fourcc::operator!=(fourcc const &f) const
{
    return data != f.data;
}

gorc::fourcc::operator uint32_t() const
{
    return data;
}
