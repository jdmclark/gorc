#pragma once

#include "token_type.hpp"
#include "text/tokenizer.hpp"

namespace gorc {

    class shell_tokenizer_state_machine : public tokenizer_state_machine {
        using tok_result = tokenizer_state_machine_result;

    public:
        enum class tokenizer_state {
            accept,
            initial,
            skip_line_comment,
            bareword,
            seen_dollar,
            seen_equal,
            seen_excl,
            string,
            escape_sequence,
            variable_name,
            bareword_variable_name,
            environment_variable_name
        };

        tokenizer_state current_state = tokenizer_state::initial;
        shell_token_type current_type = shell_token_type::error;
        std::string reason;
        std::string append_buffer;
        std::string kw_buffer;
        bool seen_whitespace = false;

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

        inline tok_result accept_immediately(shell_token_type type, maybe<char> ch = nothing)
        {
            current_type = type;
            current_state = tokenizer_state::initial;
            append_buffer.clear();
            // LCOV_EXCL_START
            // TODO: This exclusion should be removed once compound punctuators are parsed
            if(ch.has_value()) {
                append_buffer.push_back(ch.get_value());
            }
            // LCOV_EXCL_STOP

            return tok_result(tokenizer_state_machine_result_type::halt, append_buffer);
        }

        inline tok_result reject_immediately(std::string const &reason_msg)
        {
            current_type = shell_token_type::error;
            current_state = tokenizer_state::initial;
            reason = reason_msg;
            append_buffer.clear();
            return tok_result(tokenizer_state_machine_result_type::halt, append_buffer);
        }

        inline tok_result append_then_accept(char ch, shell_token_type type)
        {
            current_type = type;
            return append_directive(tokenizer_state::accept, ch);
        }

        inline tok_result skip_then_accept(shell_token_type type)
        {
            current_type = type;
            return skip_directive(tokenizer_state::accept);
        }

        tok_result handle_initial_state(char ch);
        tok_result handle_skip_line_comment_state(char ch);
        tok_result handle_bareword_state(char ch);
        tok_result handle_string_state(char ch);
        tok_result handle_escape_sequence_state(char ch);
        tok_result handle_seen_dollar_state(char ch);
        tok_result handle_seen_equal_state(char ch);
        tok_result handle_seen_excl_state(char ch);
        tok_result handle_variable_name_state(char ch);
        tok_result handle_bareword_variable_name_state(char ch);
        tok_result handle_environment_variable_name_state(char ch);

    public:
        virtual tokenizer_state_machine_result handle(char ch) override;
        virtual std::string const & get_reason() const override;
        virtual bool is_fatal_error() const override;
        shell_token_type get_type() const;
    };

    using shell_tokenizer = tokenizer<shell_tokenizer_state_machine>;
}
