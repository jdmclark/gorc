#pragma once

#include <string>
#include "io/input_stream.hpp"
#include "log/log.hpp"

namespace gorc {

    class tokenizer_stream {
    private:
        input_stream &source;
        char current_char = '\0';
        int current_line = 1;
        int current_col = 0;

        std::string current_value;
        diagnostic_context_location current_token_location;

    public:
        tokenizer_stream(input_stream &source);

        void advance_stream();
        void start_new_token();
        void append(std::string const &str);
        char peek() const;
        std::string const & get_value() const;
        diagnostic_context_location const & get_location() const;
    };

}
