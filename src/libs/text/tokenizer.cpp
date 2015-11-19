#include "tokenizer.hpp"

gorc::tokenizer_state_machine_result::tokenizer_state_machine_result(
        tokenizer_state_machine_result_type result_type,
        std::string const &value)
    : result_type(result_type)
    , value(value)
{
    return;
}

gorc::tokenizer_state_machine::~tokenizer_state_machine()
{
    return;
}

bool gorc::tokenizer_state_machine::is_fatal_error() const
{
    return false;
}
