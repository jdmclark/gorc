#include "json_output_stream.hpp"
#include <algorithm>

gorc::json_output_stream::state::state(state_mode mode)
    : mode(mode)
    , needs_comma(false)
    , awaiting_content(false)
{
    return;
}

namespace {
    gorc::service_registry default_services;
}

gorc::json_output_stream::json_output_stream(output_stream &f)
    : f(f)
    , services(default_services)
{
    states.emplace(state_mode::root);
    return;
}

gorc::json_output_stream::json_output_stream(output_stream &f,
                                             service_registry const &services)
    : f(f)
    , services(services)
{
    states.emplace(state_mode::root);
    return;
}

void gorc::json_output_stream::print_json_escaped_string(char const *str)
{
    write<char>(f, '\"');

    for(char const *it = str; *it; ++it) {
        switch(*it) {
        case '\"':
            write<char>(f, '\\');
            write<char>(f, '\"');
            break;

        case '\\':
            write<char>(f, '\\');
            write<char>(f, '\\');
            break;

        case '\n':
            write<char>(f, '\\');
            write<char>(f, 'n');
            break;

        case '\t':
            write<char>(f, '\\');
            write<char>(f, 't');
            break;

        default:
            write<char>(f, *it);
            break;
        }
    }

    write<char>(f, '\"');
}

void gorc::json_output_stream::print_integer(int64_t value)
{
    bool negative = value < 0;
    int64_t av = negative ? -value : value;

    if(negative) {
        write<char>(f, '-');
    }

    std::string buf;
    if(!value) {
        buf.push_back('0');
    }

    while(av) {
        buf.push_back(static_cast<char>('0' + (av % 10)));
        av /= 10;
    }

    std::reverse(buf.begin(), buf.end());
    f.write(buf.data(), buf.size());
}

void gorc::json_output_stream::print_double(double value)
{
    constexpr size_t bufsz = 128;
    char buffer[bufsz];
    size_t out_sz = snprintf(buffer, bufsz, "%#.16g", value) - 1;

    // Work back out_sz until first non-zero, then step forward.
    while(buffer[out_sz] == '0') {
        --out_sz;
    }

    if(buffer[out_sz] == '.') {
        // Print *.0
        ++out_sz;
    }

    f.write(buffer, out_sz + 1);
}
