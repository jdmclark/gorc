#pragma once

#include "cog_token_type.hpp"
#include "text/tokenizer.hpp"

namespace gorc {
    namespace cog {

        class cog_tokenizer_state_machine : public tokenizer_state_machine {
            using tok_result = tokenizer_state_machine_result;

        private:
            enum class tokenizer_state {
                accept,
                reject,
                initial,
                skip_line_comment,
                identifier,
                string,
                escape_sequence,
                seen_plus,
                seen_minus,
                seen_slash,
                seen_equal,
                seen_exclamation_mark,
                seen_greater,
                seen_less,
                seen_ampersand,
                seen_pipe,
                hex_prefix,
                hex_required_digit,
                hex_digit_sequence,
                digit_sequence,
                decimal_required_digit,
                decimal_digit_sequence,
                decimal_exponent_sign,
                decimal_exponent_required_digit,
                decimal_exponent_sequence,
                string_fragment
            };

            tokenizer_state current_state = tokenizer_state::initial;
            cog_token_type current_type = cog_token_type::error;
            std::string reason;
            std::string append_buffer;
            bool should_return_newlines = false;

            inline tok_result append_directive(tokenizer_state new_state,
                                               char ch)
            {
                append_buffer.clear();
                append_buffer.push_back(ch);

                current_state = new_state;

                return tok_result(tokenizer_state_machine_result_type::append, append_buffer);
            }

            inline tok_result skip_directive(tokenizer_state new_state)
            {
                append_buffer.clear();
                current_state = new_state;
                return tok_result(tokenizer_state_machine_result_type::append, append_buffer);
            }

            inline tok_result discard_directive(tokenizer_state new_state)
            {
                current_state = new_state;
                return tok_result(tokenizer_state_machine_result_type::discard, append_buffer);
            }

            inline tok_result accept_immediately(cog_token_type type, maybe<char> ch = nothing)
            {
                current_type = type;
                current_state = tokenizer_state::initial;
                append_buffer.clear();
                if(ch.has_value()) {
                    append_buffer.push_back(ch.get_value());
                }
                return tok_result(tokenizer_state_machine_result_type::halt, append_buffer);
            }

            inline tok_result reject_immediately(std::string const &reason_msg)
            {
                current_type = cog_token_type::error;
                current_state = tokenizer_state::initial;
                reason = reason_msg;
                append_buffer.clear();
                return tok_result(tokenizer_state_machine_result_type::halt, append_buffer);
            }

            inline tok_result append_then_reject(char ch,
                                                 std::string const &reason_msg)
            {
                reason = reason_msg;
                return append_directive(tokenizer_state::reject, ch);
            }

            inline tok_result append_then_accept(char ch, cog_token_type type)
            {
                current_type = type;
                return append_directive(tokenizer_state::accept, ch);
            }

            inline tok_result skip_then_accept(cog_token_type type)
            {
                current_type = type;
                return skip_directive(tokenizer_state::accept);
            }

            tok_result handle_initial_state(char ch);
            tok_result handle_skip_line_comment_state(char ch);
            tok_result handle_identifier_state(char ch);
            tok_result handle_string_state(char ch);
            tok_result handle_escape_sequence_state(char ch);
            tok_result handle_seen_plus_state(char ch);
            tok_result handle_seen_minus_state(char ch);
            tok_result handle_seen_slash_state(char ch);
            tok_result handle_seen_equal_state(char ch);
            tok_result handle_seen_exclamation_mark_state(char ch);
            tok_result handle_seen_greater_state(char ch);
            tok_result handle_seen_less_state(char ch);
            tok_result handle_seen_ampersand_state(char ch);
            tok_result handle_seen_pipe_state(char ch);
            tok_result handle_single_punctuator_state(char ch);
            tok_result handle_hex_prefix_state(char ch);
            tok_result handle_hex_required_digit_state(char ch);
            tok_result handle_hex_digit_sequence_state(char ch);
            tok_result handle_digit_sequence_state(char ch);
            tok_result handle_decimal_required_digit_state(char ch);
            tok_result handle_decimal_digit_sequence_state(char ch);
            tok_result handle_decimal_exponent_sign_state(char ch);
            tok_result handle_decimal_exponent_required_digit_state(char ch);
            tok_result handle_decimal_exponent_sequence_state(char ch);
            tok_result handle_string_fragment_state(char ch);

        public:
            virtual tokenizer_state_machine_result handle(char ch) override;
            virtual std::string const & get_reason() const override;
            virtual bool is_fatal_error() const override;

            cog_token_type get_type() const;
            void set_string_fragment_state();
            void return_newlines(bool state);
        };

        class cog_tokenizer : public tokenizer<cog_tokenizer_state_machine> {
        public:
            cog_tokenizer(input_stream &);
            void extract_string_fragment();
            void return_newlines(bool state);
        };

    }
}
