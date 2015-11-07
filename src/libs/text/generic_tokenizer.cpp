#include "generic_tokenizer.hpp"
#include "log/log.hpp"

gorc::generic_tokenizer::generic_tokenizer(input_stream &source)
    : source(source)
{
    // Initialize the token stream look-ahead
    advance();

    return;
}

void gorc::generic_tokenizer::handle_initial_state()
{
    source.start_new_token();

    char current_char = source.peek();

    if(current_char == '\0') {
        source.advance_stream();
        accept_token(token_type::end_of_file);
    }
    else if(std::isspace(current_char)) {
        source.advance_stream();
    }
    else if(current_char == '#') {
        jump_to(tokenizer_state::skip_line_comment);
    }
    else if(current_char == '_' || std::isalpha(current_char)) {
        accept_current_and_jump_to(tokenizer_state::identifier);
    }
    else if(current_char == '\"') {
        source.advance_stream();
        jump_to(tokenizer_state::string);
    }
    else if(current_char == '0') {
        accept_current_and_jump_to(tokenizer_state::hex_octal_prefix);
    }
    else if(std::isdigit(current_char)) {
        accept_current_and_jump_to(tokenizer_state::digit_sequence);
    }
    else if(std::ispunct(current_char)) {
        accept_current_and_jump_to(tokenizer_state::punctuator);
    }
    else {
        // Current character is not a recognized, valid element.
        source.advance_stream();
        reject_token("unknown input value");
    }
}

void gorc::generic_tokenizer::handle_skip_line_comment_state()
{
    switch(source.peek()) {
    case '\0':
    case '\n':
        source.advance_stream();
        jump_to(tokenizer_state::initial);
        break;

    default:
        source.advance_stream();
        break;
    }
}

void gorc::generic_tokenizer::handle_identifier_state()
{
    if(source.peek() == '_' || std::isalnum(source.peek())) {
        accept_current();
    }
    else {
        accept_token(token_type::identifier);
    }
}

void gorc::generic_tokenizer::handle_string_state()
{
    auto current_char = source.peek();
    if(current_char == '\\') {
        source.advance_stream();
        jump_to(tokenizer_state::escape_sequence);
    }
    else if(current_char == '\0') {
        source.advance_stream();
        reject_token("unexpected eof in string literal");
    }
    else if(current_char == '\"') {
        source.advance_stream();
        accept_token(token_type::string);
    }
    else {
        accept_current();
    }
}

void gorc::generic_tokenizer::handle_escape_sequence_state()
{
    switch(source.peek()) {
    case '\0':
        source.advance_stream();
        reject_token("unexpected eof in string literal escape sequence");
        return;

    case '\"':
        source.push_back('\"');
        break;

    case 'n':
        source.push_back('\n');
        break;

    case 't':
        source.push_back('\t');
        break;

    case '\\':
        source.push_back('\\');
        break;

    case '0':
        source.push_back('\0');
        break;

    default:
        reject_token(str(format("unknown escape sequence \\%c") % source.peek()));
        return;
    }

    source.advance_stream();
    jump_to(tokenizer_state::string);
}

void gorc::generic_tokenizer::handle_punctuator_state()
{
    // A dot followed by a decimal digit is a real
    if(source.get_value() == "." &&
       std::isdigit(source.peek())) {
        jump_to(tokenizer_state::decimal_required_digit);
    }
    else {
        // TODO: Multi-character punctuators
        accept_token(token_type::punctuator);
    }
}

void gorc::generic_tokenizer::handle_hex_octal_prefix_state()
{
    // Previous digit was 0.
    char current_char = source.peek();
    if(current_char == 'x' || current_char == 'X') {
        accept_current_and_jump_to(tokenizer_state::hex_required_digit);
    }
    else if(current_char == '.') {
        accept_current_and_jump_to(tokenizer_state::decimal_required_digit);
    }
    else if(current_char >= '0' && current_char <= '7') {
        accept_current_and_jump_to(tokenizer_state::oct_digit_sequence);
    }
    else if(std::isalnum(current_char)) {
        reject_token("digit out of range");
    }
    else {
        // It's just zero.
        accept_token(token_type::integer);
    }
}

void gorc::generic_tokenizer::handle_hex_required_digit_state()
{
    if(std::isxdigit(source.peek())) {
        accept_current_and_jump_to(tokenizer_state::hex_digit_sequence);
    }
    else {
        reject_token("expected hex integer");
    }
}

void gorc::generic_tokenizer::handle_hex_digit_sequence_state()
{
    if(std::isxdigit(source.peek())) {
        accept_current();
    }
    else if(std::isalnum(source.peek())) {
        reject_token("digit out of range");
    }
    else {
        accept_token(token_type::hex_integer);
    }
}

void gorc::generic_tokenizer::handle_oct_digit_sequence_state()
{
    char current_char = source.peek();
    if(current_char >= '0' && current_char <= '7') {
        accept_current();
    }
    else if(std::isalnum(current_char)) {
        reject_token("digit out of range");
    }
    else {
        accept_token(token_type::oct_integer);
    }
}

void gorc::generic_tokenizer::handle_digit_sequence_state()
{
    char current_char = source.peek();
    if(std::isdigit(current_char)) {
        accept_current();
    }
    else if(current_char == 'e' || current_char == 'E') {
        accept_current_and_jump_to(tokenizer_state::decimal_exponent_sign);
    }
    else if(current_char == '.') {
        accept_current_and_jump_to(tokenizer_state::decimal_required_digit);
    }
    else if(std::isalpha(current_char)) {
        reject_token("digit out of range");
    }
    else {
        accept_token(token_type::integer);
    }
}

void gorc::generic_tokenizer::handle_decimal_required_digit_state()
{
    if(std::isdigit(source.peek())) {
        accept_current_and_jump_to(tokenizer_state::decimal_digit_sequence);
    }
    else {
        reject_token("expected fractional part");
    }
}

void gorc::generic_tokenizer::handle_decimal_digit_sequence_state()
{
    char current_char = source.peek();
    if(std::isdigit(current_char)) {
        accept_current();
    }
    else if(current_char == 'e' || current_char == 'E') {
        accept_current_and_jump_to(tokenizer_state::decimal_exponent_sign);
    }
    else if(std::isalpha(current_char)) {
        reject_token("digit out of range");
    }
    else {
        accept_token(token_type::real);
    }
}

void gorc::generic_tokenizer::handle_decimal_exponent_sign_state()
{
    char current_char = source.peek();
    if(current_char == '+' || current_char == '-') {
        accept_current();
    }

    jump_to(tokenizer_state::decimal_exponent_required_digit);
}

void gorc::generic_tokenizer::handle_decimal_exponent_required_digit_state()
{
    if(std::isdigit(source.peek())) {
        accept_current_and_jump_to(tokenizer_state::decimal_exponent_sequence);
    }
    else {
        reject_token("expected exponent");
    }
}

void gorc::generic_tokenizer::handle_decimal_exponent_sequence_state()
{
    if(std::isdigit(source.peek())) {
        accept_current();
    }
    else if(std::isalpha(source.peek())) {
        reject_token("digit out of range");
    }
    else {
        accept_token(token_type::real);
    }
}

void gorc::generic_tokenizer::advance()
{
    current_state = tokenizer_state::initial;
    current_type = token_type::error;

    while(true) {
        switch(current_state) {
        case tokenizer_state::accept:
            return;

        case tokenizer_state::reject:
            // TODO: Throw exception?
            current_type = token_type::error;
            return;

        case tokenizer_state::initial:
            handle_initial_state();
            break;

        case tokenizer_state::skip_line_comment:
            handle_skip_line_comment_state();
            break;

        case tokenizer_state::identifier:
            handle_identifier_state();
            break;

        case tokenizer_state::string:
            handle_string_state();
            break;

        case tokenizer_state::escape_sequence:
            handle_escape_sequence_state();
            break;

        case tokenizer_state::punctuator:
            handle_punctuator_state();
            break;

        case tokenizer_state::hex_octal_prefix:
            handle_hex_octal_prefix_state();
            break;

        case tokenizer_state::hex_required_digit:
            handle_hex_required_digit_state();
            break;

        case tokenizer_state::hex_digit_sequence:
            handle_hex_digit_sequence_state();
            break;

        case tokenizer_state::oct_digit_sequence:
            handle_oct_digit_sequence_state();
            break;

        case tokenizer_state::digit_sequence:
            handle_digit_sequence_state();
            break;

        case tokenizer_state::decimal_required_digit:
            handle_decimal_required_digit_state();
            break;

        case tokenizer_state::decimal_digit_sequence:
            handle_decimal_digit_sequence_state();
            break;

        case tokenizer_state::decimal_exponent_sign:
            handle_decimal_exponent_sign_state();
            break;

        case tokenizer_state::decimal_exponent_required_digit:
            handle_decimal_exponent_required_digit_state();
            break;

        case tokenizer_state::decimal_exponent_sequence:
            handle_decimal_exponent_sequence_state();
            break;
        }
    }
}

void gorc::generic_tokenizer::extract_string_fragment()
{
    // The current stored value is usually a stripped error token.
    // Append to it until the next whitespace character.
    while(source.peek() != '\0' && !isspace(source.peek())) {
        accept_current();
    }

    current_type = token_type::string;
}
