#pragma once

#include "text/tokenizer.hpp"

namespace gorc {
    namespace cog {

        class cog_tokenizer_state_machine;

        class cog_sub_tokenizer_state_machine {
        protected:
            cog_tokenizer_state_machine &master;

        public:
            cog_sub_tokenizer_state_machine(cog_tokenizer_state_machine &master);

            virtual ~cog_sub_tokenizer_state_machine();
            virtual tokenizer_state_machine_result handle(char ch) = 0;
            virtual void reset() = 0;
        };

    }
}
