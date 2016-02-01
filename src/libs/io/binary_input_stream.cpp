#include "binary_input_stream.hpp"

gorc::binary_input_stream::binary_input_stream(input_stream &stream)
    : stream(stream)
{
    return;
}

std::string gorc::binary_input_stream::read_string()
{
    size_t amt;
    read_value<size_t>(amt);

    std::string rv;
    rv.resize(amt);

    stream.read(&rv[0], amt);

    return rv;
}

gorc::binary_deserialization_constructor::binary_deserialization_constructor()
{
    return;
}
