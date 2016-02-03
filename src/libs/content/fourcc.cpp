#include "fourcc.hpp"
#include <stdexcept>

gorc::fourcc::fourcc(deserialization_constructor_tag, binary_input_stream &bis)
    : data(binary_deserialize<uint32_t>(bis))
{
    return;
}

void gorc::fourcc::binary_serialize_object(binary_output_stream &bos) const
{
    binary_serialize(bos, data);
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

std::string gorc::to_string(fourcc f)
{
    uint32_t data = static_cast<uint32_t>(f);
    char buf[5] = { static_cast<char>(data >> 24),
                    static_cast<char>(data >> 16),
                    static_cast<char>(data >> 8),
                    static_cast<char>(data),
                    0 };
    return std::string(buf);
}
