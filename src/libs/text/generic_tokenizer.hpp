#pragma once

#include <string>
#include <sstream>
#include "io/input_stream.hpp"
#include "log/log.hpp"
#include "token_type.hpp"
#include "tokenizer_stream.hpp"

namespace gorc {

    class generic_tokenizer {
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

        tokenizer_stream source;
        tokenizer_state current_state = tokenizer_state::initial;
        token_type current_type = token_type::error;
        std::string reason;

        inline void accept_current()
        {
            source.accept_current();
        }

        inline void accept_current_and_jump_to(tokenizer_state s)
        {
            source.accept_current();
            current_state = s;
        }

        inline void accept_token(token_type tt)
        {
            current_type = tt;
            current_state = tokenizer_state::accept;
        }

        inline void reject_token(std::string const &why)
        {
            reason = why;
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
        generic_tokenizer(input_stream &);

        void advance();

        inline token_type get_type() const
        {
            return current_type;
        }

        inline std::string const & get_value() const
        {
            return source.get_value();
        }

        inline diagnostic_context_location const & get_location() const
        {
            return source.get_location();
        }

        inline std::string const & get_reason() const
        {
            return reason;
        }

        void extract_string_fragment();
    };

}
