#pragma once

#include "tokenizer_stream.hpp"

namespace gorc {

    enum class tokenizer_state_machine_result_type {
        append,
        discard,
        halt
    };

    struct tokenizer_state_machine_result {
        tokenizer_state_machine_result_type result_type;
        std::string const &value;

        tokenizer_state_machine_result(tokenizer_state_machine_result_type,
                                       std::string const &);
    };

    class tokenizer_state_machine {
    public:
        virtual ~tokenizer_state_machine();
        virtual tokenizer_state_machine_result handle(char ch) = 0;
        virtual std::string const & get_reason() const = 0;
    };

    template <typename StateMachineT>
    class tokenizer {
    protected:
        tokenizer_stream stream;
        StateMachineT state_machine;

        void advance_with_current_token()
        {
            while(true) {
                auto result = state_machine.handle(stream.peek());
                switch(result.result_type) {
                case tokenizer_state_machine_result_type::append:
                    stream.append(result.value);
                    stream.advance_stream();
                    break;

                case tokenizer_state_machine_result_type::discard:
                    stream.advance_stream();
                    stream.start_new_token();
                    break;

                case tokenizer_state_machine_result_type::halt:
                    return;
                }
            }
        }

    public:
        template <typename ...ArgT>
        tokenizer(input_stream &input, ArgT &&...arg)
            : stream(input)
            , state_machine(std::forward<ArgT>(arg)...)
        {
            // Initialize the stream token look-ahead
            advance();
            return;
        }

        virtual ~tokenizer()
        {
            return;
        }

        void advance()
        {
            stream.start_new_token();
            advance_with_current_token();
        }

        auto get_type() const -> decltype(state_machine.get_type())
        {
            return state_machine.get_type();
        }

        std::string const & get_value() const
        {
            return stream.get_value();
        }

        diagnostic_context_location const & get_location() const
        {
            return stream.get_location();
        }

        std::string const & get_reason() const
        {
            return state_machine.get_reason();
        }
    };

}
