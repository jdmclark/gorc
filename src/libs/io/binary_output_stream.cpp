#include "binary_output_stream.hpp"

gorc::binary_output_stream::binary_output_stream(output_stream &stream)
    : stream(stream)
{
    return;
}

void gorc::binary_output_stream::write_string(std::string const &str)
{
    size_t amt = str.size();
    write_value<size_t>(amt);
    stream.write(str.data(), amt);
}

size_t gorc::binary_output_stream::write_some(void const *src, size_t size)
{
    return stream.write_some(src, size);
}
