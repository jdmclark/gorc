#include "binary_input_stream.hpp"

namespace {
    gorc::service_registry default_services;
}

gorc::binary_input_stream::binary_input_stream(input_stream &stream)
    : stream(stream)
    , services(default_services)
{
    return;
}

gorc::binary_input_stream::binary_input_stream(input_stream &stream,
                                               service_registry const &services)
    : stream(stream)
    , services(services)
{
    return;
}

size_t gorc::binary_input_stream::read_some(void *dest, size_t size)
{
    return stream.read_some(dest, size);
}

bool gorc::binary_input_stream::at_end()
{
    return stream.at_end();
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
