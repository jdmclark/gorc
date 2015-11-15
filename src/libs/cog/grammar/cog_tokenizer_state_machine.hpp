#pragma once

#include "text/tokenizer.hpp"
#include "cog_token_type.hpp"
#include "cog_sub_tokenizer_state_machine.hpp"
#include <stack>

namespace gorc {
    namespace cog {

        class cog_tokenizer_state_machine : public tokenizer_state_machine {
        private:
            std::stack<cog_sub_tokenizer_state_machine*> state_stack;

        public:
            // Tokenizer states:
            // initial
            // code section
            // symbol field
            // symbol section
            // symbol def
            // symbol def assign
            // symbol ext section
            // line comment
            // string

            std::string intermediate_value_buffer;
            std::string reason;
            cog_token_type type;

            void push_state(cog_sub_tokenizer_state_machine&);
            void pop_state();

            virtual tokenizer_state_machine_result handle(char ch) override;
            virtual std::string const &get_reason() const override;
            cog_token_type get_type() const;
        };

    }
}
