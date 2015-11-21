#include "tokenizer_stream.hpp"

gorc::tokenizer_stream::tokenizer_stream(input_stream &source)
    : source(source)
    , current_token_location(nothing, 1, 1, 1, 0)
{
    // Initialize the input stream look-ahead
    advance_stream();

    return;
}

void gorc::tokenizer_stream::advance_stream()
{
    // The previous character contributed to the current token.
    current_token_location.last_line = current_line;
    current_token_location.last_col = current_col;

    if(current_char == '\n') {
        ++current_line;
        current_col = 1;
    }
    else {
        ++current_col;
    }

    try {
        source.read(&current_char, sizeof(char));
    }
    catch(...) {
        current_char = '\0';
    }
}

void gorc::tokenizer_stream::start_new_token()
{
    current_value.clear();
    current_token_location.first_line = current_token_location.last_line = current_line;
    current_token_location.first_col = current_token_location.last_col = current_col;
}

void gorc::tokenizer_stream::append(std::string const &str)
{
    current_value.append(str);
}

char gorc::tokenizer_stream::peek() const
{
    return current_char;
}

std::string const & gorc::tokenizer_stream::get_value() const
{
    return current_value;
}

gorc::diagnostic_context_location const & gorc::tokenizer_stream::get_location() const
{
    return current_token_location;
}
