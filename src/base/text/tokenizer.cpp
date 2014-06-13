#include "tokenizer.h"
#include "base/diagnostics/helper.h"
#include <boost/algorithm/string.hpp>

using namespace gorc::text;

void tokenizer::scan() {
    ++col;

    if(current == '\n') {
        ++line;
        col = 1;
    }

    current = next;
    next = buffer.get_next();
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
            out.location.last_column = col;
            diagnostics::helper::unexpected_end_of_file_in_string(ErrorReport, "tokenizer", out.location);
            out.type = token_type::invalid;
            return;
        }
        else if(current == '\n') {
            out.location.last_line = line;
            out.location.last_column = col;
            diagnostics::helper::unexpected_end_of_line_in_string(ErrorReport, "tokenizer", out.location);
            out.type = token_type::invalid;
            return;
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

tokenizer::tokenizer(source& stream, diagnostics::report& errorReport)
    : ErrorReport(errorReport), buffer(stream), report_eol(false) {
    line = 1;
    col = 1;
    current = buffer.get_next();
    next = buffer.get_next();
}

void tokenizer::skip_to_next_line() {
    while(current != '\n' && current != '\0') {
        scan();
    }
}

void tokenizer::get_token(token& out) {
    skipWhitespace();

    out.value.clear();
    out.location.filename = &buffer.filename;
    out.location.first_line = line;
    out.location.first_column = col;

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
        if(current == '-' && isdigit(next)) {
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
    out.location.last_column = col;
}

void tokenizer::get_delimited_string(token& out, const std::function<bool(char)>& match_delim) {
    skipWhitespace();

    out.value.clear();
    out.location.filename = &buffer.filename;
    out.location.first_line = line;
    out.location.first_column = col;

    while(!match_delim(current)) {
        out.value.push_back(current);
        scan();
    }

    out.type = token_type::string;
    out.location.last_line = line;
    out.location.last_column = col;

    return;
}

std::string& tokenizer::get_identifier() {
    get_token(internalToken);

    if(internalToken.type != token_type::identifier) {
        diagnostics::helper::expected(ErrorReport, "tokenizer", "identifier", internalToken.location);
        throw tokenizer_assertion_exception();
    }

    return internalToken.value;
}

std::string& tokenizer::get_string_literal() {
    get_token(internalToken);

    if(internalToken.type != token_type::string) {
        diagnostics::helper::expected_string(ErrorReport, "tokenizer", internalToken.location);
        throw tokenizer_assertion_exception();
    }

    return internalToken.value;
}

std::string& tokenizer::get_space_delimited_string() {
    get_delimited_string(internalToken, [](char c) { return isspace(c); });
    if(internalToken.value.empty()) {
        diagnostics::helper::expected(ErrorReport, "tokenizer", "filename", internalToken.location);
        throw tokenizer_assertion_exception();
    }

    return internalToken.value;
}

void tokenizer::assert_identifier(const std::string& id) {
    get_token(internalToken);

    if(internalToken.type != token_type::identifier || !boost::iequals(internalToken.value, id)) {
        diagnostics::helper::expected_identifier(ErrorReport, "tokenizer", internalToken.value, id, internalToken.location);
        throw tokenizer_assertion_exception();
    }
}

void tokenizer::assert_punctuator(const std::string& punc) {
    get_token(internalToken);

    if(internalToken.type != token_type::punctuator || internalToken.value != punc) {
        diagnostics::helper::expected_punctuator(ErrorReport, "tokenizer", internalToken.value, punc, internalToken.location);
        throw tokenizer_assertion_exception();
    }
}

void tokenizer::assert_label(const std::string& label) {
    get_token(internalToken);

    if(internalToken.type != token_type::identifier || !boost::iequals(internalToken.value, label)) {
        diagnostics::helper::expected_label(ErrorReport, "tokenizer", internalToken.value, label, internalToken.location);
        throw tokenizer_assertion_exception();
    }

    get_token(internalToken);

    if(internalToken.type != token_type::punctuator || internalToken.value != ":") {
        diagnostics::helper::expected_label(ErrorReport, "tokenizer", internalToken.value, ":", internalToken.location);
        throw tokenizer_assertion_exception();
    }
}

void tokenizer::assert_end_of_file() {
    get_token(internalToken);
    if(internalToken.type != token_type::end_of_file) {
        diagnostics::helper::expected_end_of_file(ErrorReport, "tokenizer", internalToken.value, internalToken.location);
        throw tokenizer_assertion_exception();
    }
}
