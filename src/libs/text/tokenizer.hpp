#pragma once

#include <string>
#include <sstream>
#include "io/input_stream.hpp"
#include "log/log.hpp"

namespace gorc {

    enum class token_type {
        error,
        end_of_file,
        end_of_line,
        identifier,
        string,
        integer,
        hex_integer,
        oct_integer,
        real,
        punctuator
    };

    class tokenizer {
    private:
        enum class tokenizer_state {
            accept,
            reject,
            initial,
            skip_line_comment,
            identifier,
            string,
            escape_sequence,
            punctuator,
            hex_octal_prefix,
            hex_required_digit,
            hex_digit_sequence,
            oct_digit_sequence,
            digit_sequence,
            decimal_required_digit,
            decimal_digit_sequence,
            decimal_exponent_sign,
            decimal_exponent_required_digit,
            decimal_exponent_sequence
        };

        input_stream &source;
        char current_char = '\0';
        int current_line = 1;
        int current_col = 0;

        tokenizer_state current_state = tokenizer_state::initial;
        token_type current_type = token_type::error;
        std::string current_value;
        diagnostic_context_location current_token_location;

        inline void advance_stream()
        {
            if(current_char == '\n') {
                ++current_token_location.last_line;
                current_token_location.last_col = 1;
            }
            else {
                ++current_token_location.last_col;
            }

            try {
                source.read(&current_char, sizeof(char));
            }
            catch(...) {
                current_char = '\0';
            }
        }

        inline void accept_current()
        {
            current_value.push_back(current_char);
            advance_stream();
        }

        inline void accept_current_and_jump_to(tokenizer_state s)
        {
            accept_current();
            current_state = s;
        }

        inline void accept_token(token_type tt)
        {
            current_type = tt;
            current_state = tokenizer_state::accept;
        }

        inline void reject_token(std::string const &reason)
        {
            current_value = reason;
            current_state = tokenizer_state::reject;
        }

        inline void jump_to(tokenizer_state s)
        {
            current_state = s;
        }

        void handle_initial_state();
        void handle_skip_line_comment_state();
        void handle_identifier_state();
        void handle_string_state();
        void handle_escape_sequence_state();
        void handle_punctuator_state();
        void handle_hex_octal_prefix_state();
        void handle_hex_required_digit_state();
        void handle_hex_digit_sequence_state();
        void handle_oct_digit_sequence_state();
        void handle_digit_sequence_state();
        void handle_decimal_required_digit_state();
        void handle_decimal_digit_sequence_state();
        void handle_decimal_exponent_sign_state();
        void handle_decimal_exponent_required_digit_state();
        void handle_decimal_exponent_sequence_state();

    public:
        tokenizer(input_stream &);

        void advance();

        inline token_type get_type() const
        {
            return current_type;
        }

        inline std::string const & get_value() const
        {
            return current_value;
        }

        inline diagnostic_context_location const & get_location() const
        {
            return current_token_location;
        }
    };

}
