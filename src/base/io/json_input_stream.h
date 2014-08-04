#pragma once

#include <cstdio>
#include <type_traits>
#include <iterator>
#include <stack>
#include "input_stream.h"
#include "output_stream.h"
#include "file.h"
#include "exception.h"

namespace gorc {
namespace io {

class json_input_stream {
    template <typename EmFmtFn> friend void json_deserialize_members(json_input_stream& f, EmFmtFn efn);
    template <typename ValueT, typename OutItT> friend void json_deserialize_array(json_input_stream& f, const OutItT& rng);
    template <typename OutItT, typename EmFmtFn> friend void json_deserialize_array(json_input_stream& f, const OutItT& rng, EmFmtFn efn);
    friend void json_deserialize_null(json_input_stream& f);
    template <typename T> friend typename std::enable_if<std::is_same<std::string, T>::value, T>::type json_deserialize(json_input_stream& f);
    template <typename T> friend typename std::enable_if<std::is_fundamental<T>::value, T>::type json_deserialize(json_input_stream& f);

private:
    input_stream& f;
    char next_ch;
    std::string curr_token;

    inline void advance_char() {
        if(f.at_end()) {
            next_ch = '\0';
        }
        else {
            try {
                next_ch = read<char>(f);
            }
            catch(io::file_read_exception&) {
                // Reached end of stream.
                next_ch = '\0';
            }
        }
    }

    inline char next() {
        char tmp = next_ch;
        advance_char();
        return tmp;
    }

    void advance_token();

    inline void assert_token(const char* value) {
        if(curr_token != value) {
            throw json_invalid_format_exception();
        }

        advance_token();
    }

    inline bool at_end() {
        return curr_token.empty();
    }

    int64_t read_integer();
    double read_double();

    template <typename T> typename std::enable_if<std::is_same<T, bool>::value, T>::type
    read_value() {
        if(curr_token == "true") {
            advance_token();
            return true;
        }
        else if(curr_token == "false") {
            advance_token();
            return false;
        }
        else {
            throw json_invalid_format_exception();
        }
    }

    template <typename T> typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, T>::type
    read_value() {
        int64_t value = read_integer();
        advance_token();
        return static_cast<T>(value);
    }

    template <typename T> typename std::enable_if<std::is_floating_point<T>::value, T>::type read_value() {
        double value = read_double();
        advance_token();
        return static_cast<T>(value);
    }

public:
    explicit json_input_stream(input_stream& f);
};

struct json_constructor_tag { } const json_constructor;

template <typename T> typename std::enable_if<std::is_fundamental<T>::value, T>::type json_deserialize(json_input_stream& f) {
    return f.read_value<T>();
}

template <typename T> typename std::enable_if<std::is_same<std::string, T>::value, T>::type json_deserialize(json_input_stream& f) {
    std::string rv = f.curr_token;
    f.advance_token();
    return rv;
}

template <typename T> typename std::enable_if<!std::is_fundamental<T>::value && !std::is_same<std::string, T>::value>::type
    json_deserialize(json_input_stream& f, T& obj) {
    obj.json_deserialize(f);
}

template <typename T> typename std::enable_if<!std::is_fundamental<T>::value && !std::is_same<std::string, T>::value, T>::type
    json_deserialize(json_input_stream& f) {
    T obj(json_constructor, f);
    return obj;
}

inline void json_deserialize_null(json_input_stream& f) {
    f.assert_token("null");
}

template <typename OutItT, typename EmFmtFn> void json_deserialize_array(json_input_stream& f, const OutItT& rng, EmFmtFn efn) {
    auto out_it = rng;

    f.assert_token("[");

    while(!f.at_end()) {
        if(f.curr_token == "]") {
            f.advance_token();
            return;
        }
        else if(f.curr_token == ",") {
            f.advance_token();
            continue;
        }
        else {
            *out_it = efn();
            ++out_it;
        }
    }

    throw json_invalid_format_exception();
}

template <typename ValueT, typename OutItT> void json_deserialize_array(json_input_stream& f, const OutItT& rng) {
    auto out_it = rng;

    f.assert_token("[");

    while(!f.at_end()) {
        if(f.curr_token == "]") {
            f.advance_token();
            return;
        }
        else if(f.curr_token == ",") {
            f.advance_token();
            continue;
        }
        else {
            *out_it = json_deserialize<ValueT>(f);
            ++out_it;
        }
    }

    throw json_invalid_format_exception();
}

template <typename EmFmtFn> void json_deserialize_members(json_input_stream& f, EmFmtFn efn) {
    f.assert_token("{");

    std::string name_buf;

    while(!f.at_end()) {
        if(f.curr_token == "}") {
            f.advance_token();
            return;
        }
        else if(f.curr_token == ",") {
            f.advance_token();
            continue;
        }
        else {
            // Current token is a member name.
            name_buf = f.curr_token;
            f.advance_token();
            f.assert_token(":");
            efn(name_buf);
        }
    }

    throw json_invalid_format_exception();
}

}
}
