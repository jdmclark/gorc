#include "cog_tokenizer_state_machine.hpp"
#include "utility/runtime_assert.hpp"

using namespace gorc::cog;

void cog_tokenizer_state_machine::push_state(cog_sub_tokenizer_state_machine &state)
{
    state.reset();
    state_stack.push(&state);
}

void cog_tokenizer_state_machine::pop_state()
{
    state_stack.pop();
}

gorc::tokenizer_state_machine_result cog_tokenizer_state_machine::handle(char ch)
{
    runtime_assert(!state_stack.empty(), "cog_tokenizer_state_machine stack empty");
    return state_stack.top()->handle(ch);
}

std::string const & cog_tokenizer_state_machine::get_reason() const
{
    return reason;
}

cog_token_type cog_tokenizer_state_machine::get_type() const
{
    return type;
}
