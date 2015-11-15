#include "cog_sub_tokenizer_state_machine.hpp"

gorc::cog::cog_sub_tokenizer_state_machine::cog_sub_tokenizer_state_machine(
        cog_tokenizer_state_machine &master)
    : master(master)
{
    return;
}

gorc::cog::cog_sub_tokenizer_state_machine::~cog_sub_tokenizer_state_machine()
{
    return;
}
