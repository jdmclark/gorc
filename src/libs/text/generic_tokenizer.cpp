#include "generic_tokenizer.hpp"
#include "log/log.hpp"

using tok_result = gorc::tokenizer_state_machine_result;

tok_result gorc::generic_tokenizer_state_machine::handle_initial_state(char current_char)
{
    if(current_char == '\0') {
        return append_then_accept(current_char, token_type::end_of_file);
    }
    else if(current_char == '\n' && return_newlines) {
        return skip_then_accept(token_type::end_of_line);
    }
    else if(std::isspace(current_char)) {
        return discard_directive(tokenizer_state::initial);
    }
    else if(current_char == '#') {
        return discard_directive(tokenizer_state::skip_line_comment);
    }
    else if(current_char == '_' || std::isalpha(current_char)) {
        return append_directive(tokenizer_state::identifier, current_char);
    }
    else if(current_char == '\"') {
        return skip_directive(tokenizer_state::string);
    }
    else if(current_char == '0') {
        return append_directive(tokenizer_state::hex_octal_prefix, current_char);
    }
    else if(std::isdigit(current_char)) {
        return append_directive(tokenizer_state::digit_sequence, current_char);
    }
    else if(current_char == '.') {
        return append_directive(tokenizer_state::period, current_char);
    }
    else if(std::ispunct(current_char)) {
        return append_then_accept(current_char, token_type::punctuator);
    }
    else {
        // Current character is not a recognized, valid element.
        return append_then_reject(current_char, "unknown input value");
    }
}

tok_result gorc::generic_tokenizer_state_machine::handle_skip_line_comment_state(char ch)
{
    if(ch == '\n' && return_newlines) {
        return skip_then_accept(token_type::end_of_line);
    }
    else if(ch == '\0' || ch == '\n') {
        return discard_directive(tokenizer_state::initial);
    }
    else {
        return discard_directive(tokenizer_state::skip_line_comment);
    }
}

tok_result gorc::generic_tokenizer_state_machine::handle_identifier_state(char ch)
{
    if(ch == '_' || std::isalnum(ch)) {
        return append_directive(tokenizer_state::identifier, ch);
    }
    else {
        return accept_immediately(token_type::identifier);
    }
}

tok_result gorc::generic_tokenizer_state_machine::handle_string_state(char current_char)
{
    if(current_char == '\\') {
        return skip_directive(tokenizer_state::escape_sequence);
    }
    else if(current_char == '\0') {
        return reject_immediately("unexpected eof in string literal");
    }
    else if(current_char == '\"') {
        return skip_then_accept(token_type::string);
    }
    else {
        return append_directive(tokenizer_state::string, current_char);
    }
}

tok_result gorc::generic_tokenizer_state_machine::handle_escape_sequence_state(char ch)
{
    // Return to string state, unless there is an error
    char append_char = '\0';

    switch(ch) {
    case '\0':
        return reject_immediately("unexpected eof in string literal escape sequence");

    case '\"':
        append_char = '\"';
        break;

    case 'n':
        append_char = '\n';
        break;

    case 't':
        append_char = '\t';
        break;

    case '\\':
        append_char = '\\';
        break;

    case '0':
        append_char = '\0';
        break;

    default:
        return reject_immediately(str(format("unknown escape sequence \\%c") % ch));
    }

    return append_directive(tokenizer_state::string, append_char);
}

tok_result gorc::generic_tokenizer_state_machine::handle_period_state(char ch)
{
    // A dot followed by a decimal digit is a real
    if(std::isdigit(ch)) {
        return append_directive(tokenizer_state::decimal_digit_sequence, ch);
    }
    else {
        return accept_immediately(token_type::punctuator);
    }
}

tok_result gorc::generic_tokenizer_state_machine::handle_hex_octal_prefix_state(char current_char)
{
    // Previous digit was 0.
    if(current_char == 'x' || current_char == 'X') {
        return append_directive(tokenizer_state::hex_required_digit, current_char);
    }
    else if(current_char == '.') {
        return append_directive(tokenizer_state::decimal_required_digit, current_char);
    }
    else if(current_char >= '0' && current_char <= '7') {
        return append_directive(tokenizer_state::oct_digit_sequence, current_char);
    }
    else if(std::isalnum(current_char)) {
        return reject_immediately("digit out of range");
    }
    else {
        // It's just zero.
        return accept_immediately(token_type::integer);
    }
}

tok_result gorc::generic_tokenizer_state_machine::handle_hex_required_digit_state(char ch)
{
    if(std::isxdigit(ch)) {
        return append_directive(tokenizer_state::hex_digit_sequence, ch);
    }
    else {
        return reject_immediately("expected hex integer");
    }
}

tok_result gorc::generic_tokenizer_state_machine::handle_hex_digit_sequence_state(char ch)
{
    if(std::isxdigit(ch)) {
        return append_directive(tokenizer_state::hex_digit_sequence, ch);
    }
    else if(std::isalnum(ch)) {
        return reject_immediately("digit out of range");
    }
    else {
        return accept_immediately(token_type::hex_integer);
    }
}

tok_result gorc::generic_tokenizer_state_machine::handle_oct_digit_sequence_state(char ch)
{
    if(ch >= '0' && ch <= '7') {
        return append_directive(tokenizer_state::oct_digit_sequence, ch);
    }
    else if(std::isalnum(ch)) {
        return reject_immediately("digit out of range");
    }
    else {
        return accept_immediately(token_type::oct_integer);
    }
}

tok_result gorc::generic_tokenizer_state_machine::handle_digit_sequence_state(char current_char)
{
    if(std::isdigit(current_char)) {
        return append_directive(tokenizer_state::digit_sequence, current_char);
    }
    else if(current_char == 'e' || current_char == 'E') {
        return append_directive(tokenizer_state::decimal_exponent_sign, current_char);
    }
    else if(current_char == '.') {
        return append_directive(tokenizer_state::decimal_required_digit, current_char);
    }
    else if(std::isalpha(current_char)) {
        return reject_immediately("digit out of range");
    }
    else {
        return accept_immediately(token_type::integer);
    }
}

tok_result gorc::generic_tokenizer_state_machine::handle_decimal_required_digit_state(char ch)
{
    if(std::isdigit(ch)) {
        return append_directive(tokenizer_state::decimal_digit_sequence, ch);
    }
    else {
        return reject_immediately("expected fractional part");
    }
}

tok_result gorc::generic_tokenizer_state_machine::handle_decimal_digit_sequence_state(char ch)
{
    if(std::isdigit(ch)) {
        return append_directive(tokenizer_state::decimal_digit_sequence, ch);
    }
    else if(ch == 'e' || ch == 'E') {
        return append_directive(tokenizer_state::decimal_exponent_sign, ch);
    }
    else if(std::isalpha(ch)) {
        return reject_immediately("digit out of range");
    }
    else {
        return accept_immediately(token_type::real);
    }
}

tok_result gorc::generic_tokenizer_state_machine::handle_decimal_exponent_sign_state(char ch)
{
    if(ch == '+' || ch == '-') {
        return append_directive(tokenizer_state::decimal_exponent_required_digit, ch);
    }
    else {
        return handle_decimal_exponent_required_digit_state(ch);
    }
}

tok_result gorc::generic_tokenizer_state_machine::handle_decimal_exponent_required_digit_state(char ch)
{
    if(std::isdigit(ch)) {
        return append_directive(tokenizer_state::decimal_exponent_sequence, ch);
    }
    else {
        return reject_immediately("expected exponent");
    }
}

tok_result gorc::generic_tokenizer_state_machine::handle_decimal_exponent_sequence_state(char ch)
{
    if(std::isdigit(ch)) {
        return append_directive(tokenizer_state::decimal_exponent_sequence, ch);
    }
    else if(std::isalpha(ch)) {
        return reject_immediately("digit out of range");
    }
    else {
        return accept_immediately(token_type::real);
    }
}

tok_result gorc::generic_tokenizer_state_machine::handle_string_fragment_state(char ch)
{
    if(isspace(ch) || ch == '\0') {
        return accept_immediately(token_type::string);
    }

    return append_directive(tokenizer_state::string_fragment, ch);
}

tok_result gorc::generic_tokenizer_state_machine::handle(char ch)
{
    switch(current_state) {
    case tokenizer_state::accept:
        current_state = tokenizer_state::initial;
        append_buffer.clear();
        return tok_result(tokenizer_state_machine_result_type::halt, append_buffer);

    default:
    case tokenizer_state::reject:
        // TODO: Throw exception?
        current_state = tokenizer_state::initial;
        current_type = token_type::error;
        append_buffer.clear();
        return tok_result(tokenizer_state_machine_result_type::halt, append_buffer);

    case tokenizer_state::initial:
        return handle_initial_state(ch);

    case tokenizer_state::skip_line_comment:
        return handle_skip_line_comment_state(ch);

    case tokenizer_state::identifier:
        return handle_identifier_state(ch);

    case tokenizer_state::string:
        return handle_string_state(ch);

    case tokenizer_state::escape_sequence:
        return handle_escape_sequence_state(ch);

    case tokenizer_state::period:
        return handle_period_state(ch);

    case tokenizer_state::hex_octal_prefix:
        return handle_hex_octal_prefix_state(ch);

    case tokenizer_state::hex_required_digit:
        return handle_hex_required_digit_state(ch);

    case tokenizer_state::hex_digit_sequence:
        return handle_hex_digit_sequence_state(ch);

    case tokenizer_state::oct_digit_sequence:
        return handle_oct_digit_sequence_state(ch);

    case tokenizer_state::digit_sequence:
        return handle_digit_sequence_state(ch);

    case tokenizer_state::decimal_required_digit:
        return handle_decimal_required_digit_state(ch);

    case tokenizer_state::decimal_digit_sequence:
        return handle_decimal_digit_sequence_state(ch);

    case tokenizer_state::decimal_exponent_sign:
        return handle_decimal_exponent_sign_state(ch);

    case tokenizer_state::decimal_exponent_required_digit:
        return handle_decimal_exponent_required_digit_state(ch);

    case tokenizer_state::decimal_exponent_sequence:
        return handle_decimal_exponent_sequence_state(ch);

    case tokenizer_state::string_fragment:
        return handle_string_fragment_state(ch);
    }
}

gorc::token_type gorc::generic_tokenizer_state_machine::get_type() const
{
    return current_type;
}

std::string const & gorc::generic_tokenizer_state_machine::get_reason() const
{
    return reason;
}

void gorc::generic_tokenizer_state_machine::set_string_fragment_state()
{
    current_state = tokenizer_state::string_fragment;
}

void gorc::generic_tokenizer_state_machine::set_return_newlines(bool value)
{
    return_newlines = value;
}

gorc::generic_tokenizer::generic_tokenizer(input_stream &input)
    : tokenizer(input)
{
    return;
}

void gorc::generic_tokenizer::extract_string_fragment()
{
    // The current stored value is usually a stripped error token.
    // Append to it until the next whitespace character.
    state_machine.set_string_fragment_state();
    advance_with_current_token();
}

void gorc::generic_tokenizer::return_newlines(bool value)
{
    state_machine.set_return_newlines(value);
}
