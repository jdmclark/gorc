#include "lexer.hpp"
#include "log/log.hpp"

using namespace gorc;
using namespace gorc::cog;
using tok_result = gorc::tokenizer_state_machine_result;

tok_result cog_tokenizer_state_machine::handle_initial_state(char current_char)
{
    if(current_char == '\0') {
        return accept_immediately(cog_token_type::end_of_file);
    }
    else if(should_return_newlines && current_char == '\n') {
        return append_then_accept(current_char, cog_token_type::end_of_line);
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
        return append_directive(tokenizer_state::hex_prefix, current_char);
    }
    else if(std::isdigit(current_char)) {
        return append_directive(tokenizer_state::digit_sequence, current_char);
    }
    else if(current_char == '/') {
        return discard_directive(tokenizer_state::seen_slash);
    }
    else if(current_char == '=') {
        return append_directive(tokenizer_state::seen_equal, current_char);
    }
    else if(current_char == '!') {
        return append_directive(tokenizer_state::seen_exclamation_mark, current_char);
    }
    else if(current_char == '>') {
        return append_directive(tokenizer_state::seen_greater, current_char);
    }
    else if(current_char == '<') {
        return append_directive(tokenizer_state::seen_less, current_char);
    }
    else if(current_char == '&') {
        return append_directive(tokenizer_state::seen_ampersand, current_char);
    }
    else if(current_char == '|') {
        return append_directive(tokenizer_state::seen_pipe, current_char);
    }
    else if(current_char == '+') {
        return append_then_accept(current_char, cog_token_type::punc_plus);
    }
    else if(current_char == '-') {
        return append_then_accept(current_char, cog_token_type::punc_minus);
    }
    else if(current_char == '.') {
        return append_directive(tokenizer_state::seen_dot, current_char);
    }
    else if(std::ispunct(current_char)) {
        return handle_single_punctuator_state(current_char);
    }
    else {
        // Current character is not a recognized, valid element.
        int chval = *reinterpret_cast<unsigned char*>(&current_char);
        return reject_immediately(str(format("illegal character 0x%X") % chval));
    }
}

tok_result cog_tokenizer_state_machine::handle_skip_line_comment_state(char ch)
{
    if(should_return_newlines && ch == '\n') {
        return append_then_accept(ch, cog_token_type::end_of_line);
    }
    else if(ch == '\0' || ch == '\n') {
        return discard_directive(tokenizer_state::initial);
    }
    else {
        return discard_directive(tokenizer_state::skip_line_comment);
    }
}

tok_result cog_tokenizer_state_machine::handle_identifier_state(char ch)
{
    if(ch == '_' || std::isalnum(ch)) {
        return append_directive(tokenizer_state::identifier, ch);
    }
    else {
        return accept_immediately(cog_token_type::identifier);
    }
}

tok_result cog_tokenizer_state_machine::handle_string_state(char current_char)
{
    if(current_char == '\\') {
        return skip_directive(tokenizer_state::escape_sequence);
    }
    else if(current_char == '\0') {
        return reject_immediately("unexpected eof in string literal");
    }
    else if(current_char == '\n') {
        return reject_immediately("unescaped newline in string literal");
    }
    else if(current_char == '\"') {
        return skip_then_accept(cog_token_type::string);
    }
    else {
        return append_directive(tokenizer_state::string, current_char);
    }
}

tok_result cog_tokenizer_state_machine::handle_escape_sequence_state(char ch)
{
    // Return to string state unless there is an error
    char append_char = '\0';

    switch(ch) {
    case '\0':
        return reject_immediately("unexpected eof in string literal escape sequence");

    case '\"':
        append_char = '\"';
        break;

    case '\\':
        append_char = '\\';
        break;

    case 'n':
        append_char = '\n';
        break;

    case '\n':
        // Consume escaped newlines
        return skip_directive(tokenizer_state::string);

    default:
        return reject_immediately(str(format("unknown escape sequence '\\%c'") % ch));
    }

    return append_directive(tokenizer_state::string, append_char);
}

tok_result cog_tokenizer_state_machine::handle_seen_dot_state(char ch)
{
    if(std::isdigit(ch)) {
        return append_directive(tokenizer_state::decimal_digit_sequence, ch);
    }
    else {
        return reject_immediately("unrecognized punctuator '.'");
    }
}

tok_result cog_tokenizer_state_machine::handle_seen_slash_state(char ch)
{
    // A slash followed by another slash is a line comment
    if(ch == '/') {
        return skip_directive(tokenizer_state::skip_line_comment);
    }
    else {
        return accept_immediately(cog_token_type::punc_div, '/');
    }
}

tok_result cog_tokenizer_state_machine::handle_seen_equal_state(char ch)
{
    if(ch == '=') {
        return append_then_accept(ch, cog_token_type::punc_equal);
    }
    else {
        return accept_immediately(cog_token_type::punc_assign);
    }
}

tok_result cog_tokenizer_state_machine::handle_seen_exclamation_mark_state(char ch)
{
    if(ch == '=') {
        return append_then_accept(ch, cog_token_type::punc_notequal);
    }
    else {
        return accept_immediately(cog_token_type::punc_excl);
    }
}

tok_result cog_tokenizer_state_machine::handle_seen_greater_state(char ch)
{
    if(ch == '=') {
        return append_then_accept(ch, cog_token_type::punc_greaterequal);
    }
    else {
        return accept_immediately(cog_token_type::punc_greater);
    }
}

tok_result cog_tokenizer_state_machine::handle_seen_less_state(char ch)
{
    if(ch == '=') {
        return append_then_accept(ch, cog_token_type::punc_lessequal);
    }
    else {
        return accept_immediately(cog_token_type::punc_less);
    }
}

tok_result cog_tokenizer_state_machine::handle_seen_ampersand_state(char ch)
{
    if(ch == '&') {
        return append_then_accept(ch, cog_token_type::punc_logical_and);
    }
    else {
        return accept_immediately(cog_token_type::punc_and);
    }
}

tok_result cog_tokenizer_state_machine::handle_seen_pipe_state(char ch)
{
    if(ch == '|') {
        return append_then_accept(ch, cog_token_type::punc_logical_or);
    }
    else {
        return accept_immediately(cog_token_type::punc_or);
    }
}

tok_result cog_tokenizer_state_machine::handle_single_punctuator_state(char ch)
{
    switch(ch) {
    case ':':
        return append_then_accept(ch, cog_token_type::punc_colon);

    case ';':
        return append_then_accept(ch, cog_token_type::punc_semicolon);

    case ',':
        return append_then_accept(ch, cog_token_type::punc_comma);

    case '\'':
        return append_then_accept(ch, cog_token_type::punc_apos);

    case '*':
        return append_then_accept(ch, cog_token_type::punc_times);

    case '%':
        return append_then_accept(ch, cog_token_type::punc_mod);

    case '^':
        return append_then_accept(ch, cog_token_type::punc_xor);

    case '{':
        return append_then_accept(ch, cog_token_type::punc_lbrace);

    case '}':
        return append_then_accept(ch, cog_token_type::punc_rbrace);

    case '(':
        return append_then_accept(ch, cog_token_type::punc_lparen);

    case ')':
        return append_then_accept(ch, cog_token_type::punc_rparen);

    case '[':
        return append_then_accept(ch, cog_token_type::punc_lbracket);

    case ']':
        return append_then_accept(ch, cog_token_type::punc_rbracket);

    default:
        // Current character is not a recognized, valid element.
        return reject_immediately(str(format("unrecognized punctuator '%c'") % ch));
    }
}

tok_result cog_tokenizer_state_machine::handle_hex_prefix_state(char current_char)
{
    // Previous digit was 0.
    if(current_char == 'x' || current_char == 'X') {
        return append_directive(tokenizer_state::hex_required_digit, current_char);
    }
    else if(current_char == '.') {
        return append_directive(tokenizer_state::decimal_required_digit, current_char);
    }
    else if(std::isdigit(current_char)) {
        // cog doesn't allow octal integers - treat this as any other
        return append_directive(tokenizer_state::digit_sequence, current_char);
    }
    else {
        // It's just zero.
        return accept_immediately(cog_token_type::integer);
    }
}

tok_result cog_tokenizer_state_machine::handle_hex_required_digit_state(char ch)
{
    if(std::isxdigit(ch)) {
        return append_directive(tokenizer_state::hex_digit_sequence, ch);
    }
    else {
        return reject_immediately("expected hex digit");
    }
}

tok_result cog_tokenizer_state_machine::handle_hex_digit_sequence_state(char ch)
{
    if(std::isxdigit(ch)) {
        return append_directive(tokenizer_state::hex_digit_sequence, ch);
    }
    else if(std::isalnum(ch)) {
        return reject_immediately("digit out of range");
    }
    else {
        return accept_immediately(cog_token_type::hex_integer);
    }
}

tok_result cog_tokenizer_state_machine::handle_digit_sequence_state(char current_char)
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
        return accept_immediately(cog_token_type::integer);
    }
}

tok_result cog_tokenizer_state_machine::handle_decimal_required_digit_state(char ch)
{
    if(std::isdigit(ch)) {
        return append_directive(tokenizer_state::decimal_digit_sequence, ch);
    }
    else {
        return reject_immediately("expected fractional part");
    }
}

tok_result cog_tokenizer_state_machine::handle_decimal_digit_sequence_state(char ch)
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
        return accept_immediately(cog_token_type::real);
    }
}

tok_result cog_tokenizer_state_machine::handle_decimal_exponent_sign_state(char ch)
{
    if(ch == '+' || ch == '-') {
        return append_directive(tokenizer_state::decimal_exponent_required_digit, ch);
    }
    else if(std::isdigit(ch)) {
        return append_directive(tokenizer_state::decimal_exponent_sequence, ch);
    }
    else {
        return reject_immediately("expected exponent");
    }
}

tok_result cog_tokenizer_state_machine::handle_decimal_exponent_required_digit_state(char ch) {
    if(std::isdigit(ch)) {
        return append_directive(tokenizer_state::decimal_exponent_sequence, ch);
    }
    else {
        return reject_immediately("expected exponent");
    }
}

tok_result cog_tokenizer_state_machine::handle_decimal_exponent_sequence_state(char ch) {
    if(std::isdigit(ch)) {
        return append_directive(tokenizer_state::decimal_exponent_sequence, ch);
    }
    else if(std::isalpha(ch)) {
        return reject_immediately("digit out of range");
    }
    else {
        return accept_immediately(cog_token_type::real);
    }
}

tok_result cog_tokenizer_state_machine::handle_sf_initial_state(char ch)
{
    if(ch == '\0') {
        return accept_immediately(cog_token_type::end_of_file);
    }
    else if(std::isspace(ch)) {
        return discard_directive(tokenizer_state::sf_initial);
    }
    else if(ch == '+' || ch == '-') {
        // Maybe a number
        return append_directive(tokenizer_state::sf_seen_sign, ch);
    }
    else if(ch == '.') {
        // Maybe decimal
        return append_directive(tokenizer_state::sf_decimal_required_digit, ch);
    }
    else if(ch == '0') {
        // Maybe hex, maybe decimal, maybe zero
        return append_directive(tokenizer_state::sf_hex_prefix, ch);
    }
    else if(std::isdigit(ch)) {
        // Maybe integer, maybe decimal
        return append_directive(tokenizer_state::sf_digit_sequence, ch);
    }
    else {
        return append_directive(tokenizer_state::sf_string_fragment, ch);
    }
}

tok_result cog_tokenizer_state_machine::handle_sf_seen_sign_state(char ch)
{
    if(ch == '\0' || std::isspace(ch)) {
        // Just + or -
        return accept_immediately(cog_token_type::string);
    }
    else if(ch == '.') {
        // Maybe decimal
        return append_directive(tokenizer_state::sf_decimal_required_digit, ch);
    }
    else if(ch == '0') {
        // Maybe hex, maybe decimal, maybe zero
        return append_directive(tokenizer_state::sf_hex_prefix, ch);
    }
    else if(std::isdigit(ch)) {
        // Maybe integer, maybe decimal
        return append_directive(tokenizer_state::sf_digit_sequence, ch);
    }
    else {
        return append_directive(tokenizer_state::sf_string_fragment, ch);
    }
}

tok_result cog_tokenizer_state_machine::handle_sf_hex_prefix_state(char ch)
{
    if(ch == '\0' || std::isspace(ch)) {
        // Just zero
        return accept_immediately(cog_token_type::integer);
    }
    else if(ch == 'x' || ch == 'X') {
        // This is a hex sequence
        return append_directive(tokenizer_state::sf_hex_required_digit, ch);
    }
    else if(ch == '.') {
        // 0.
        return append_directive(tokenizer_state::sf_decimal_digit_sequence, ch);
    }
    else if(ch == 'e' || ch == 'E') {
        // 0e
        return append_directive(tokenizer_state::sf_decimal_exponent_sign, ch);
    }
    else if(std::isdigit(ch)) {
        // An integer, although not a very well written one
        return append_directive(tokenizer_state::sf_digit_sequence, ch);
    }
    else {
        return append_directive(tokenizer_state::sf_string_fragment, ch);
    }
}

tok_result cog_tokenizer_state_machine::handle_sf_hex_required_digit_state(char ch)
{
    if(ch == '\0' || std::isspace(ch)) {
        // "0x"
        return accept_immediately(cog_token_type::string);
    }
    else if(std::isxdigit(ch)) {
        return append_directive(tokenizer_state::sf_hex_digit_sequence, ch);
    }
    else {
        return append_directive(tokenizer_state::sf_string_fragment, ch);
    }
}

tok_result cog_tokenizer_state_machine::handle_sf_hex_digit_sequence_state(char ch)
{
    if(ch == '\0' || std::isspace(ch)) {
        return accept_immediately(cog_token_type::hex_integer);
    }
    else if(std::isxdigit(ch)) {
        return append_directive(tokenizer_state::sf_hex_digit_sequence, ch);
    }
    else {
        return append_directive(tokenizer_state::sf_string_fragment, ch);
    }
}

tok_result cog_tokenizer_state_machine::handle_sf_digit_sequence_state(char ch)
{
    if(ch == '\0' || std::isspace(ch)) {
        return accept_immediately(cog_token_type::integer);
    }
    else if(std::isdigit(ch)) {
        return append_directive(tokenizer_state::sf_digit_sequence, ch);
    }
    else if(ch == 'e' || ch == 'E') {
        return append_directive(tokenizer_state::sf_decimal_exponent_sign, ch);
    }
    else if(ch == '.') {
        // Already seen at least one number before the dot.
        // Be more permissive.
        return append_directive(tokenizer_state::sf_decimal_digit_sequence, ch);
    }
    else {
        return append_directive(tokenizer_state::sf_string_fragment, ch);
    }
}

tok_result cog_tokenizer_state_machine::handle_sf_decimal_required_digit_state(char ch)
{
    if(ch == '\0' || std::isspace(ch)) {
        // No digits seen - sign and dot
        return accept_immediately(cog_token_type::string);
    }
    else if(std::isdigit(ch)) {
        return append_directive(tokenizer_state::sf_decimal_digit_sequence, ch);
    }
    else {
        // No digits seen
        return append_directive(tokenizer_state::sf_string_fragment, ch);
    }
}

tok_result cog_tokenizer_state_machine::handle_sf_decimal_digit_sequence_state(char ch)
{
    if(ch == '\0' || std::isspace(ch)) {
        // At least one digit seen
        return accept_immediately(cog_token_type::real);
    }
    else if(std::isdigit(ch)) {
        return append_directive(tokenizer_state::sf_decimal_digit_sequence, ch);
    }
    else if(ch == 'e' || ch == 'E') {
        return append_directive(tokenizer_state::sf_decimal_exponent_sign, ch);
    }
    else {
        return append_directive(tokenizer_state::sf_string_fragment, ch);
    }
}

tok_result cog_tokenizer_state_machine::handle_sf_decimal_exponent_sign_state(char ch)
{
    if(ch == '\0' || std::isspace(ch)) {
        // Not a valid real
        return accept_immediately(cog_token_type::string);
    }
    else if(ch == '+' || ch == '-') {
        return append_directive(tokenizer_state::sf_decimal_exponent_required_digit, ch);
    }
    else if(std::isdigit(ch)) {
        return append_directive(tokenizer_state::sf_decimal_exponent_sequence, ch);
    }
    else {
        return append_directive(tokenizer_state::sf_string_fragment, ch);
    }
}

tok_result cog_tokenizer_state_machine::handle_sf_decimal_exponent_required_digit_state(char ch)
{
    if(ch == '\0' || std::isspace(ch)) {
        // Not a valid real
        return accept_immediately(cog_token_type::string);
    }
    else if(std::isdigit(ch)) {
        return append_directive(tokenizer_state::sf_decimal_exponent_sequence, ch);
    }
    else {
        return append_directive(tokenizer_state::sf_string_fragment, ch);
    }
}

tok_result cog_tokenizer_state_machine::handle_sf_decimal_exponent_sequence_state(char ch)
{
    if(ch == '\0' || std::isspace(ch)) {
        return accept_immediately(cog_token_type::real);
    }
    else if(std::isdigit(ch)) {
        return append_directive(tokenizer_state::sf_decimal_exponent_sequence, ch);
    }
    else {
        return append_directive(tokenizer_state::sf_string_fragment, ch);
    }
}

tok_result cog_tokenizer_state_machine::handle_sf_string_fragment_state(char ch)
{
    if(ch == '\0' || isspace(ch)) {
        return accept_immediately(cog_token_type::string);
    }
    else {
        return append_directive(tokenizer_state::sf_string_fragment, ch);
    }
}

tok_result cog_tokenizer_state_machine::handle(char ch)
{
    switch(current_state) {
    case tokenizer_state::accept:
        current_state = tokenizer_state::initial;
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

    case tokenizer_state::seen_dot:
        return handle_seen_dot_state(ch);

    case tokenizer_state::seen_slash:
        return handle_seen_slash_state(ch);

    case tokenizer_state::seen_equal:
        return handle_seen_equal_state(ch);

    case tokenizer_state::seen_exclamation_mark:
        return handle_seen_exclamation_mark_state(ch);

    case tokenizer_state::seen_greater:
        return handle_seen_greater_state(ch);

    case tokenizer_state::seen_less:
        return handle_seen_less_state(ch);

    case tokenizer_state::seen_ampersand:
        return handle_seen_ampersand_state(ch);

    case tokenizer_state::seen_pipe:
        return handle_seen_pipe_state(ch);

    case tokenizer_state::hex_prefix:
        return handle_hex_prefix_state(ch);

    case tokenizer_state::hex_required_digit:
        return handle_hex_required_digit_state(ch);

    case tokenizer_state::hex_digit_sequence:
        return handle_hex_digit_sequence_state(ch);

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

    case tokenizer_state::sf_initial:
        return handle_sf_initial_state(ch);

    case tokenizer_state::sf_seen_sign:
        return handle_sf_seen_sign_state(ch);

    case tokenizer_state::sf_hex_prefix:
        return handle_sf_hex_prefix_state(ch);

    case tokenizer_state::sf_hex_required_digit:
        return handle_sf_hex_required_digit_state(ch);

    case tokenizer_state::sf_hex_digit_sequence:
        return handle_sf_hex_digit_sequence_state(ch);

    case tokenizer_state::sf_digit_sequence:
        return handle_sf_digit_sequence_state(ch);

    case tokenizer_state::sf_decimal_required_digit:
        return handle_sf_decimal_required_digit_state(ch);

    case tokenizer_state::sf_decimal_digit_sequence:
        return handle_sf_decimal_digit_sequence_state(ch);

    case tokenizer_state::sf_decimal_exponent_sign:
        return handle_sf_decimal_exponent_sign_state(ch);

    case tokenizer_state::sf_decimal_exponent_required_digit:
        return handle_sf_decimal_exponent_required_digit_state(ch);

    case tokenizer_state::sf_decimal_exponent_sequence:
        return handle_sf_decimal_exponent_sequence_state(ch);

    case tokenizer_state::sf_string_fragment:
        return handle_sf_string_fragment_state(ch);

    // LCOV_EXCL_START
    }

    LOG_FATAL("unhandled cog tokenizer state");
}
// LCOV_EXCL_STOP

cog_token_type cog_tokenizer_state_machine::get_type() const
{
    return current_type;
}

std::string const& cog_tokenizer_state_machine::get_reason() const
{
    return reason;
}

bool cog_tokenizer_state_machine::is_fatal_error() const
{
    return current_type == cog_token_type::error;
}

void cog_tokenizer_state_machine::set_symbol_field_state()
{
    current_state = tokenizer_state::sf_initial;
}

void cog_tokenizer_state_machine::return_newlines(bool state)
{
    should_return_newlines = state;
}

cog_tokenizer::cog_tokenizer(input_stream &input)
    : tokenizer(input)
{
    return;
}

void cog_tokenizer::set_symbol_field_state()
{
    state_machine.set_symbol_field_state();
}

void cog_tokenizer::return_newlines(bool state)
{
    state_machine.return_newlines(state);
}
