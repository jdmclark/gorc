#include "log/log.hpp"
#include "tokenizer.hpp"
#include <boost/algorithm/string.hpp>

using namespace gorc::text;

char tokenizer::buffer_get_next() {
    while(!buffer.at_end()) {
        char ch;
        auto amt = buffer.read_some(&ch, sizeof(char));
        if(amt == sizeof(char)) {
            return ch;
        }
    }

    return '\0';
}

void tokenizer::scan() {
    ++col;

    if(current == '\n') {
        ++line;
        col = 1;
    }

    current = next;
    next = buffer_get_next();
}

bool tokenizer::skipWhitespaceStep() {
    if(current == '\0') {
        // EOF in a block of whitespace
        return false;
    }
    else if(report_eol && current == '\n') {
        return false;
    }
    else if(isspace(current)) {
        scan();
        return true;
    }
    else if(current == '#') {
        // Line comment, skip to eol.
        skip_to_next_line();
        return true;
    }
    else {
        return false;
    }
}

void tokenizer::skipWhitespace() {
    while(skipWhitespaceStep()) {
        // Repeatedly call skipWhitespaceStep() until it returns false.
        // Returning false indicates no more whitespace to skip.
    }

    return;
}

bool tokenizer::isIdentifierLead(char c) {
    return isalpha(c) || (c == '_');
}

bool tokenizer::isIdentifierChar(char c) {
    return isalnum(c) || (c == '_');
}

void tokenizer::readNumericLiteralHexPart(token& out) {
    while(isxdigit(current)) {
        out.value.push_back(current);
        scan();
    }
}

void tokenizer::readNumericLiteralIntegerPart(token& out) {
    while(isdigit(current)) {
        out.value.push_back(current);
        scan();
    }
}

void tokenizer::readNumericLiteral(token& out) {
    // Check for sign:
    if(current == '-') {
        out.value.push_back(current);
        scan();
    }

    if(current == '0' && (next == 'x' || next == 'X')) {
        out.type = token_type::hex_integer;

        out.value.push_back(current);
        out.value.push_back(next);

        scan();
        scan();

        readNumericLiteralHexPart(out);
        return;
    }

    out.type = token_type::integer;

    readNumericLiteralIntegerPart(out);

    if(current == '.' && isdigit(next)) {
        if(out.value.empty() || !isdigit(out.value.back())) {
            // Poorly formatted floating point number. Prepend 0.
            out.value.push_back('0');
        }

        out.value.push_back(current);
        scan();

        readNumericLiteralIntegerPart(out);

        out.type = token_type::floating;
    }

    if(current == 'e' || current == 'E') {
        out.value.push_back(current);
        scan();

        if(current == '+' || current == '-') {
            out.value.push_back(current);
            scan();
        }

        readNumericLiteralIntegerPart(out);
        out.type = token_type::floating;
    }
}

void tokenizer::readIdentifier(token& out) {
    if(isIdentifierLead(current)) {
        // Valid identifier
        out.type = token_type::identifier;

        do {
            out.value.push_back(current);
            scan();
        } while(isIdentifierChar(current));
    }
}

void tokenizer::readStringLiteral(token& out) {
    while(true) {
        scan();

        if(current == '\0') {
            out.location.last_line = line;
            out.location.last_col = col;
            diagnostic_context dc(nullptr, line, col);
            LOG_FATAL("unexpected end of file in string literal");
        }
        else if(current == '\n') {
            out.location.last_line = line;
            out.location.last_col = col;
            diagnostic_context dc(nullptr, line, col);
            LOG_FATAL("unexpected newline in string literal");
        }
        else if(current == '\"') {
            out.type = token_type::string;
            scan();
            return;
        }
        else if(current == '\\') {
            // Escape sequence.
            scan();

            switch(current) {
            case '\n':
                // Escaped newline
                break;

            case '\'':
            case '\"':
            case '\\':
                out.value.push_back(current);
                break;

            case 'n':
                out.value.push_back('\n');
                break;

            case 't':
                out.value.push_back('\t');
                break;

            default:
                out.type = token_type::invalid;
                return;
            }
        }
        else {
            out.value.push_back(current);
        }
    }
}

tokenizer::tokenizer(input_stream& stream)
    : buffer(stream), report_eol(false) {
    line = 1;
    col = 1;
    current = buffer_get_next();
    next = buffer_get_next();
}

void tokenizer::skip_to_next_line() {
    while(current != '\n' && current != '\0') {
        scan();
    }
}

void tokenizer::get_token(token& out) {
    skipWhitespace();

    out.value.clear();
    out.location.first_line = line;
    out.location.first_col = col;

    if(current == '\0') {
        // Stream has reached end of file.
        out.type = token_type::end_of_file;
    }
    else if(current == '\n') {
        out.type = token_type::end_of_line;
        scan();
    }
    else if(current == '\"') {
        readStringLiteral(out);
    }
    else if(isIdentifierLead(current)) {
        readIdentifier(out);
    }
    else if(isdigit(current)) {
        readNumericLiteral(out);
    }
    else if(ispunct(current)) {
        if(current == '-' && (next == '.' || isdigit(next))) {
            readNumericLiteral(out);
        }
        else if(current == '.' && isdigit(next)) {
            readNumericLiteral(out);
        }
        else {
            out.value.push_back(current);
            out.type = token_type::punctuator;
            scan();
        }
    }
    else {
        out.type = token_type::invalid;
    }

    out.location.last_line = line;
    out.location.last_col = col;
}

void tokenizer::get_delimited_string(token& out, const std::function<bool(char)>& match_delim) {
    skipWhitespace();

    out.value.clear();
    out.location.first_line = line;
    out.location.first_col = col;

    while(!match_delim(current)) {
        out.value.push_back(current);
        scan();
    }

    out.type = token_type::string;
    out.location.last_line = line;
    out.location.last_col = col;

    return;
}

std::string& tokenizer::get_identifier() {
    get_token(internalToken);

    if(internalToken.type != token_type::identifier) {
        diagnostic_context dc(nullptr, internalToken.location.first_line, internalToken.location.first_col);
        LOG_FATAL("expected identifier");
    }

    return internalToken.value;
}

std::string& tokenizer::get_string_literal() {
    get_token(internalToken);

    if(internalToken.type != token_type::string) {
        diagnostic_context dc(nullptr, internalToken.location.first_line, internalToken.location.first_col);
        LOG_FATAL("expected string literal");
    }

    return internalToken.value;
}

std::string& tokenizer::get_space_delimited_string() {
    get_delimited_string(internalToken, [](char c) { return isspace(c); });
    if(internalToken.value.empty()) {
        diagnostic_context dc(nullptr, internalToken.location.first_line, internalToken.location.first_col);
        LOG_FATAL("expected string fragment");
    }

    return internalToken.value;
}

void tokenizer::assert_identifier(const std::string& id) {
    get_token(internalToken);

    if(internalToken.type != token_type::identifier || !boost::iequals(internalToken.value, id)) {
        diagnostic_context dc(nullptr, internalToken.location.first_line, internalToken.location.first_col);
        LOG_FATAL(format("expected '%s', found '%s'") % id % internalToken.value);
    }
}

void tokenizer::assert_punctuator(const std::string& punc) {
    get_token(internalToken);

    if(internalToken.type != token_type::punctuator || internalToken.value != punc) {
        diagnostic_context dc(nullptr, internalToken.location.first_line, internalToken.location.first_col);
        LOG_FATAL(format("expected '%s', found '%s'") % punc % internalToken.value);
    }
}

void tokenizer::assert_label(const std::string& label) {
    assert_identifier(label);
    assert_punctuator(":");
}

void tokenizer::assert_end_of_file() {
    get_token(internalToken);
    if(internalToken.type != token_type::end_of_file) {
        LOG_FATAL("expected end of file");
    }
}
